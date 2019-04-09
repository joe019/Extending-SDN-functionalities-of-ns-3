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

//                Controller
//                    *
//                  *   *
//                *       *  
//       ----------    -----------
//       | Switch1 |---| Switch2 |
//       ----------    ----------- 
//        |      |      |       | 
//        n1     n2     n3      n4
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
  terminals.Create (4);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (2);

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices1;
  NetDeviceContainer switchDevices2;
   

  // Connection between switches
  NetDeviceContainer linkSwitch = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(1)));
  switchDevices1.Add (linkSwitch.Get (0));
  switchDevices2.Add (linkSwitch.Get (1));

 // n1 and n2 connected to switch1
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch.Get(0)));
      terminalDevices.Add (link.Get (0));
      switchDevices1.Add (link.Get (1));
    }  
  
   // n3 and n4 connected to switch2
  for(int i=2; i<4; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch.Get(1)));
      terminalDevices.Add (link.Get (0));
      switchDevices2.Add (link.Get (1));
  }
  
  Ptr<Node> switchNode1 = csmaSwitch.Get (0);
  Ptr<Node> switchNode2 = csmaSwitch.Get (1);
  OpenFlowSwitchHelper switch1,switch2;

  // Creating the controller
  Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
   if (!timeout.IsZero ()) controller->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  //Linking switch1 with controller
  switch1.Install (switchNode1, switchDevices1, controller);
  
  //Linking switch1 with controller
  switch2.Install (switchNode2, switchDevices2, controller);
 
  InternetStackHelper internet;
  internet.Install (terminals);
    
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);

  // Create an OnOff application to send UDP datagrams from n0 to n1.
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)

  OnOffHelper onoff ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port)));
  onoff.SetConstantRate (DataRate ("1kb/s"));

  ApplicationContainer app = onoff.Install (terminals.Get (0));
  // Start the application
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));

  // // Create an optional packet sink to receive these packets
  // PacketSinkHelper sink ("ns3::UdpSocketFactory",
  //                        Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  // app = sink.Install (terminals.Get (1));
  // app.Start (Seconds (0.0));

  // NS_LOG_INFO ("Configure Tracing.");

  // //
  // // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // // Trace output will be sent to the file "openflow-switch.tr"
  // //
  // AsciiTraceHelper ascii;
  // csma.EnableAsciiAll (ascii.CreateFileStream ("openflow-switch.tr"));

  // //
  // // Also configure some tcpdump traces; each interface will be traced.
  // // The output files will be named:
  // //     openflow-switch-<nodeId>-<interfaceId>.pcap
  // // and can be read by the "tcpdump -r" command (use "-tt" option to
  // // display timestamps correctly)
  // //
  // csma.EnablePcapAll ("openflow-switch", false);

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}
