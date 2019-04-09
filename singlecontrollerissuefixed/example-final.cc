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
#include "ns3/v4ping-helper.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OpenFlowCsmaSwitchExample");

bool verbose = false;
bool use_drop = false;
ns3::Time timeout = ns3::Seconds (0);

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
  terminals.Create (2);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (3);

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices1;
  // NetDeviceContainer terminalDevices2;
  NetDeviceContainer switchDevices2;
  NetDeviceContainer switchDevices3;
 
  OpenFlowSwitchHelper swtch1;
  OpenFlowSwitchHelper swtch2;
  OpenFlowSwitchHelper swtch3;

  NetDeviceContainer linkSwitchA = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(1)));
  NetDeviceContainer linkSwitchB = csma.Install (NodeContainer (csmaSwitch.Get(1),csmaSwitch.Get(2)));
  // NetDeviceContainer linkSwitchC = csma.Install (NodeContainer (csmaSwitch.Get(2),csmaSwitch.Get(0)));//making straight topology

  //switchDevices1.Add (linkSwitchA.Get (0));

  switchDevices1=swtch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(1)->GetDevice(0),linkSwitchA.Get(0));


 // std::map<uint32_t,Mac48Address>::iterator st = switchdevicemap1.find(0);

  //switchDevices1.Add (linkSwitchC.Get (1));

 // switchDevices2.Add (linkSwitchA.Get (1));

  switchDevices2=swtch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(0)->GetDevice(0),linkSwitchA.Get(1));
 // switchDevices2.Add (linkSwitchB.Get (0));
  switchDevices2=swtch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(2)->GetDevice(0),linkSwitchB.Get(0));

  //switchDevices3.Add (linkSwitchB.Get (1));
  switchDevices3=swtch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(1)->GetDevice(0),linkSwitchB.Get(1));
  NS_LOG_INFO ("tingtingting"<<Mac48Address::ConvertFrom(csmaSwitch.Get(2)->GetDevice(0)->GetAddress()));

  //switchDevices3.Add (linkSwitchC.Get (0));

  NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (0), csmaSwitch.Get(0)));
  terminalDevices.Add (link.Get (0));
  //switchDevices1.Add (link.Get (1));

  switchDevices1=swtch1.addDeviceNode(switchDevices1,terminals.Get (0)->GetDevice(0),link.Get(1));

 // NS_LOG_INFO ("device_added"<<Mac48Address::ConvertFrom(((link.Get(1))->GetDevice(0))->GetAddress()));



  link = csma.Install (NodeContainer (terminals.Get (1), csmaSwitch.Get(2)));
  terminalDevices.Add (link.Get (0));
 // switchDevices3.Add (link.Get (1));

  switchDevices3=swtch3.addDeviceNode(switchDevices3,terminals.Get (1)->GetDevice(0),link.Get(1));


  NS_LOG_INFO ("switch_confusion"<<Mac48Address::ConvertFrom(((csmaSwitch.Get(2))->GetDevice(0))->GetAddress()));
  // Create the switch netdevice, which will do the packet switching
  /*
  for(int i=2; i<4; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch.Get(1)));
      terminalDevices.Add (link.Get (0));
      switchDevices2.Add (link.Get (1));
  }
  */
  //create controller
  Ptr<ns3::ofi::LearningController> controllerA = CreateObject<ns3::ofi::LearningController> ();
  //Ptr<ns3::ofi::DropController> controllerA = CreateObject<ns3::ofi::DropController> ();.
   if (!timeout.IsZero ()) controllerA->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  // Ptr<ns3::ofi::LearningController> controllerB = CreateObject<ns3::ofi::LearningController> ();
  //  if (!timeout.IsZero ()) controllerB->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  // Ptr<ns3::ofi::LearningController> controllerC = CreateObject<ns3::ofi::LearningController> ();
  //  if (!timeout.IsZero ()) controllerC->SetAttribute ("ExpirationTime", TimeValue (timeout));

 //create openflow switch
  Ptr<Node> switchNode1 = csmaSwitch.Get (0);
  

  Ptr<Node> switchNode2 = csmaSwitch.Get (1);


  Ptr<Node> switchNode3 = csmaSwitch.Get (2);

  
  //install cntroller in switches
  swtch1.Install (switchNode1, switchDevices1, controllerA);
  swtch2.Install (switchNode2, switchDevices2, controllerA);
  swtch3.Install (switchNode3, switchDevices3, controllerA);


  
  // PointToPointHelper pointToPoint;
  // pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  // pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // NetDeviceContainer p2pDevices;
  // p2pDevices = pointToPoint.Install (csmaSwitch);


  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);
  
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer addresses=ipv4.Assign (terminalDevices);

  //todo
  //Ping application to be converted to configure function



  /*Mac48Address src_addr=Mac48Address::ConvertFrom(((terminals.Get(0))->GetDevice(0))->GetAddress());
  NS_LOG_INFO ("mac_address"<<src_addr);
  NS_LOG_INFO ("Create pinger");
  NS_LOG_INFO ("No of devices"<<terminals.GetN());
  for(uint32_t i1=0;i1<terminals.GetN();++i1)
  {
	  for(uint32_t j1=i1+1;j1<terminals.GetN();++j1)
	  {
		  V4PingHelper ping = V4PingHelper(addresses.GetAddress (i1));
	  	  ApplicationContainer app = ping.Install (terminals.Get (j1));
	  	  app.Start (Seconds (2.0));
	  	  app.Stop (Seconds (2.0));
	  }


=======
  }*/

  //creation of UDP traffic


  // ipv4.Assign(terminalDevices2);
   
  // Ipv4AddressHelper ipv42; 
  // ipv42.SetBase ("10.1.2.0", "255.255.255.0");
  // ipv42.Assign (terminalDevices2);
  
  // // Create an OnOff application to send UDP datagrams from n0 to n1.
  // NS_LOG_INFO ("Create Applications.");
  // uint16_t port = 9;   // Discard port (RFC 863)

  // OnOffHelper onoff ("ns3::UdpSocketFactory",
  //                    Address (InetSocketAddress (Ipv4Address ("10.1.1.0"), port)));
  // onoff.SetConstantRate (DataRate ("5kb/s"));

  // ApplicationContainer app = onoff.Install (terminals.Get (2));
  // // Start the application
  // app.Start (Seconds (1.0));
  // app.Stop (Seconds (10.0));

  // // Create an optional packet sink to receive these packets
  // PacketSinkHelper sink ("ns3::UdpSocketFactory",
  //                        Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  // app = sink.Install (terminals.Get (1));
  // app.Start (Seconds (0.0));

  // app = sink.Install (terminals.Get (0));
  // app.Start (Seconds (0.0));




  //creation of TCP traffic


    NS_LOG_INFO ("Create Applications.");


//Create a BulkSendApplication and install it on node 0

  uint16_t port = 9;  // well-known echo port number

  //terminals.Get(0).Ipv4Address()
 BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address("10.1.1.2"), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (1));
  ApplicationContainer sourceApps = source.Install (terminals.Get (0));
  sourceApps.Start (Seconds (1.0));

  sourceApps.Stop (Seconds (10.0));


//
// Create a PacketSinkApplication and install it on node 1
//
/*  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer sinkApps = sink.Install (terminals.Get (1));
  sinkApps.Start (Seconds (2.0));
  sinkApps.Stop (Seconds (2.0));*/

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
/*  for(int i1=0;i1<100;++i1)
  NS_LOG_INFO ("\n\n\n\n\n\n\n\n\n\n");*/
  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}
