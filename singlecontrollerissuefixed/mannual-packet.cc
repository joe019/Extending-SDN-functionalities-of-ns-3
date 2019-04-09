/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Network topology
//
//        n0     n1
//        |      |
//       ----------
//       | Switch |
//       ----------
//        |      |
//        n2     n3
//
//
// - CBR/UDP flows from n0 to n1 and from n3 to n0
// - DropTail queues
// - Tracing of queues and packet receptions to file "openflow-switch.tr"
// - If order of adding nodes and netdevices is kept:
//      n0 = 00:00:00;00:00:01, n1 = 00:00:00:00:00:03, n3 = 00:00:00:00:00:07
//	and port number corresponds to node number, so port 0 is connected to n0, for example.

#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/log.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OpenFlowCsmaSwitchExample");

bool verbose = false;
bool use_drop = false;
ns3::Time timeout = ns3::Seconds (0);
void
ServerConnectionEstablished (Ptr<const ThreeGppHttpServer>, Ptr<Socket>)
{
  NS_LOG_INFO ("Client has established a connection to the server.");
}

void
MainObjectGenerated (uint32_t size)
{
  NS_LOG_INFO ("Server generated a main object of " << size << " bytes.");
}

void
EmbeddedObjectGenerated (uint32_t size)
{
  NS_LOG_INFO ("Server generated an embedded object of " << size << " bytes.");
}

void
ServerTx (Ptr<const Packet> packet)
{
  NS_LOG_INFO ("Server sent a packet of " << packet->GetSize () << " bytes.");
}

void
ClientRx (Ptr<const Packet> packet, const Address &address)
{
  NS_LOG_INFO ("Client received a packet of " << packet->GetSize () << " bytes from " << address);
}

void
ClientMainObjectReceived (Ptr<const ThreeGppHttpClient>, Ptr<const Packet> packet)
{
  Ptr<Packet> p = packet->Copy ();
  ThreeGppHttpHeader header;
  p->RemoveHeader (header);
  if (header.GetContentLength () == p->GetSize ()
      && header.GetContentType () == ThreeGppHttpHeader::MAIN_OBJECT)
    {
      NS_LOG_INFO ("Client has successfully received a main object of "
                   << p->GetSize () << " bytes.");
    }
  else
    {
      NS_LOG_INFO ("Client failed to parse a main object. ");
    }
}

void
ClientEmbeddedObjectReceived (Ptr<const ThreeGppHttpClient>, Ptr<const Packet> packet)
{
  Ptr<Packet> p = packet->Copy ();
  ThreeGppHttpHeader header;
  p->RemoveHeader (header);
  if (header.GetContentLength () == p->GetSize ()
      && header.GetContentType () == ThreeGppHttpHeader::EMBEDDED_OBJECT)
    {
      NS_LOG_INFO ("Client has successfully received an embedded object of "
                   << p->GetSize () << " bytes.");
    }
  else
    {
      NS_LOG_INFO ("Client failed to parse an embedded object. ");
    }
}
bool
SetVerbose (std::string value)
{
  verbose = true;
  return true;
}

bool
SetDrop (std::string value)
{
  use_drop = true;
  return true;
}

bool
SetTimeout (std::string value)
{
  try {
      timeout = ns3::Seconds (atof (value.c_str ()));
      return true;
    }
  catch (...) { return false; }
  return false;
}
void ReceivePacket (Ptr<Socket> socket)
 {
   NS_LOG_INFO ("Received one packet!");
   Ptr<Packet> packet = socket->Recv ();
   SocketIpTosTag tosTag;
   if (packet->RemovePacketTag (tosTag))
     {
       NS_LOG_INFO (" TOS = " << (uint32_t)tosTag.GetTos ());
     }
   SocketIpTtlTag ttlTag;
   if (packet->RemovePacketTag (ttlTag))
     {
       NS_LOG_INFO (" TTL = " << (uint32_t)ttlTag.GetTtl ());
     }
 }

 static void SendPacket (Ptr<Socket> socket, uint32_t pktSize,
                         uint32_t pktCount, Time pktInterval )
 {
   if (pktCount > 0)
     {
       socket->Send (Create<Packet> (pktSize));
       Simulator::Schedule (pktInterval, &SendPacket,
                            socket, pktSize,pktCount - 1, pktInterval);
     }
   else
     {
       socket->Close ();
     }
 }
int
main (int argc, char *argv[])
{
  #ifdef NS3_OPENFLOW
  //
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  //
  CommandLine cmd;
  cmd.AddValue ("v", "Verbose (turns on logging).", MakeCallback (&SetVerbose));
  cmd.AddValue ("verbose", "Verbose (turns on logging).", MakeCallback (&SetVerbose));
  cmd.AddValue ("d", "Use Drop Controller (Learning if not specified).", MakeCallback (&SetDrop));
  cmd.AddValue ("drop", "Use Drop Controller (Learning if not specified).", MakeCallback (&SetDrop));
  cmd.AddValue ("t", "Learning Controller Timeout (has no effect if drop controller is specified).", MakeCallback ( &SetTimeout));
  cmd.AddValue ("timeout", "Learning Controller Timeout (has no effect if drop controller is specified).", MakeCallback ( &SetTimeout));

  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("OpenFlowCsmaSwitchExample", LOG_LEVEL_INFO);
      LogComponentEnable ("OpenFlowInterface", LOG_LEVEL_INFO);
      LogComponentEnable ("OpenFlowSwitchNetDevice", LOG_LEVEL_INFO);
    }

  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (3);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (3);

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices1;
  // NetDeviceContainer terminalDevices2;
  NetDeviceContainer switchDevices2;
  
NetDeviceContainer switchDevices3;
 
   


  NetDeviceContainer linkSwitchA = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(1)));
  NetDeviceContainer linkSwitchB = csma.Install (NodeContainer (csmaSwitch.Get(1),csmaSwitch.Get(2)));
  NetDeviceContainer linkSwitchC = csma.Install (NodeContainer (csmaSwitch.Get(2),csmaSwitch.Get(0)));

  switchDevices1.Add (linkSwitchA.Get (0));
  switchDevices1.Add (linkSwitchC.Get (1));

  switchDevices2.Add (linkSwitchA.Get (1));
  switchDevices2.Add (linkSwitchB.Get (0));

  switchDevices3.Add (linkSwitchB.Get (1));
  switchDevices3.Add (linkSwitchC.Get (0));

  
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (0), csmaSwitch.Get(0)));
      terminalDevices.Add (link.Get (0));
      switchDevices1.Add (link.Get (1));
    
      link = csma.Install (NodeContainer (terminals.Get (1), csmaSwitch.Get(1)));
      terminalDevices.Add (link.Get (0));
      switchDevices2.Add (link.Get (1));

      link = csma.Install (NodeContainer (terminals.Get (2), csmaSwitch.Get(2)));
      terminalDevices.Add (link.Get (0));
      switchDevices3.Add (link.Get (1));
    

  // Create the switch netdevice, which will do the packet switching
  
  
/*  
  for(int i=2; i<4; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch.Get(1)));
      terminalDevices.Add (link.Get (0));
      switchDevices2.Add (link.Get (1));
  }
  */

  Ptr<ns3::ofi::LearningController> controllerA = CreateObject<ns3::ofi::LearningController> ();
   if (!timeout.IsZero ()) controllerA->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  Ptr<ns3::ofi::LearningController> controllerB = CreateObject<ns3::ofi::LearningController> ();
   if (!timeout.IsZero ()) controllerB->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  Ptr<ns3::ofi::LearningController> controllerC = CreateObject<ns3::ofi::LearningController> ();
   if (!timeout.IsZero ()) controllerC->SetAttribute ("ExpirationTime", TimeValue (timeout));

 
  Ptr<Node> switchNode1 = csmaSwitch.Get (0);
  OpenFlowSwitchHelper swtch1;
  

  Ptr<Node> switchNode2 = csmaSwitch.Get (1);
  OpenFlowSwitchHelper swtch2;

  Ptr<Node> switchNode3 = csmaSwitch.Get (2);
  OpenFlowSwitchHelper swtch3;
  
  
  swtch1.Install (switchNode1, switchDevices1, controllerA);
  swtch2.Install (switchNode2, switchDevices2, controllerB);
  swtch3.Install (switchNode3, switchDevices3, controllerC);

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);
  
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);

   
NS_LOG_INFO ("Create Applications.");






//UDP Socket
  //uint16_t port1 = 443;   // Discard port (RFC 863)
  //uint16_t port2 = 21;   // Discard port (RFC 863)

/*
  OnOffHelper onoff ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.3"), port1)));
  onoff.SetConstantRate (DataRate ("5kb/s"));

  ApplicationContainer app1 = onoff.Install (terminals.Get (0));
  // Start the application
  app1.Start (Seconds (1.0));
  app1.Stop (Seconds (2.0));*/
  uint32_t packetSize = 1024;
  uint32_t packetCount = 10;
  double packetInterval = 1.0;

  //Socket options for IPv4, currently TOS, TTL, RECVTOS, and RECVTTL
  uint32_t ipTos = 0;
  bool ipRecvTos = true;
  uint32_t ipTtl = 0;
 bool ipRecvTtl = true;



  NS_LOG_INFO ("Create sockets.");
  //Receiver socket on n1
  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (terminals.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address ("10.1.1.3"), 80);
  recvSink->SetIpRecvTos (ipRecvTos);
    recvSink->SetIpRecvTtl (ipRecvTtl);
    recvSink->Bind (local);

  //Sender socket on n0
  Ptr<Socket> source = Socket::CreateSocket (terminals.Get (1), tid);
 InetSocketAddress remote = InetSocketAddress (Ipv4Address ("10.1.1.1"), 81);
 if (ipTos > 0)
     {
       source->SetIpTos (ipTos);
     }

   if (ipTtl > 0)
     {
       source->SetIpTtl (ipTtl);
  }


   source->Connect (remote);
 //Schedule SendPacket
    Time interPacketInterval = Seconds (packetInterval);
    Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                    Seconds (1.0), &SendPacket,
   source, packetSize, packetCount, interPacketInterval);
/*
// Create HTTP server helper
ThreeGppHttpServerHelper serverHelper (Ipv4Address ("10.1.1.3"));

// Install HTTP server
ApplicationContainer serverApps = serverHelper.Install (terminals.Get (0));
Ptr<ThreeGppHttpServer> httpServer = serverApps.Get (0)->GetObject<ThreeGppHttpServer> ();

// Example of connecting to the trace sources
httpServer->TraceConnectWithoutContext ("ConnectionEstablished",
                                        MakeCallback (&ServerConnectionEstablished));
httpServer->TraceConnectWithoutContext ("MainObject", MakeCallback (&MainObjectGenerated));
httpServer->TraceConnectWithoutContext ("EmbeddedObject", MakeCallback (&EmbeddedObjectGenerated));
httpServer->TraceConnectWithoutContext ("Tx", MakeCallback (&ServerTx));

// Setup HTTP variables for the server
PointerValue varPtr;
httpServer->GetAttribute ("Variables", varPtr);
Ptr<ThreeGppHttpVariables> httpVariables = varPtr.Get<ThreeGppHttpVariables> ();
httpVariables->SetMainObjectSizeMean (102400); // 100kB
httpVariables->SetMainObjectSizeStdDev (40960); // 40kB


// Create HTTP client helper
ThreeGppHttpClientHelper clientHelper (Ipv4Address ("10.1.1.3"));

// Install HTTP client
ApplicationContainer clientApps = clientHelper.Install (terminals.Get (1));
Ptr<ThreeGppHttpClient> httpClient = clientApps.Get (0)->GetObject<ThreeGppHttpClient> ();

// Example of connecting to the trace sources
httpClient->TraceConnectWithoutContext ("RxMainObject", MakeCallback (&ClientMainObjectReceived));
httpClient->TraceConnectWithoutContext ("RxEmbeddedObject", MakeCallback (&ClientEmbeddedObjectReceived));
httpClient->TraceConnectWithoutContext ("Rx", MakeCallback (&ClientRx));

// Stop browsing after 30 minutes
clientApps.Stop (Seconds (10));
*/














  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}
