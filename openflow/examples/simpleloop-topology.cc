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
  NetDeviceContainer switchDevices2;
  NetDeviceContainer switchDevices3;
 
  OpenFlowSwitchHelper switch1;
  OpenFlowSwitchHelper switch2;
  OpenFlowSwitchHelper switch3;

  NetDeviceContainer linkSwitchA = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(1)));
  NetDeviceContainer linkSwitchB = csma.Install (NodeContainer (csmaSwitch.Get(1),csmaSwitch.Get(2)));
  NetDeviceContainer linkSwitchC = csma.Install (NodeContainer (csmaSwitch.Get(2),csmaSwitch.Get(0)));

  switchDevices1=switch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(1)->GetDevice(0),linkSwitchA.Get(0));
  switchDevices2=switch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(0)->GetDevice(0),linkSwitchA.Get(1));
 
  switchDevices2=switch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(2)->GetDevice(0),linkSwitchB.Get(0));
  switchDevices3=switch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(1)->GetDevice(0),linkSwitchB.Get(1));

  switchDevices3=switch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(2)->GetDevice(0),linkSwitchC.Get(0));
  switchDevices1=switch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(0)->GetDevice(0),linkSwitchC.Get(1));
 

  NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (0), csmaSwitch.Get(0)));
  terminalDevices.Add (link.Get (0));
  switchDevices1=switch1.addDeviceNode(switchDevices1,terminals.Get (0)->GetDevice(0),link.Get(1));

  link = csma.Install (NodeContainer (terminals.Get (1), csmaSwitch.Get(2)));
  terminalDevices.Add (link.Get (0));
  switchDevices3=switch3.addDeviceNode(switchDevices3,terminals.Get (1)->GetDevice(0),link.Get(1));

  //create controller
  Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
  if (!timeout.IsZero ()) controller->SetAttribute ("ExpirationTime", TimeValue (timeout));
    
  Ptr<Node> switchNode1 = csmaSwitch.Get (0);
  Ptr<Node> switchNode2 = csmaSwitch.Get (1);
  Ptr<Node> switchNode3 = csmaSwitch.Get (2);

  // Switches set to high traffic
  switch1.toggleTrafficFlag();
  switch2.toggleTrafficFlag();
  
  //install cntroller in switches
  switch1.Install (switchNode1, switchDevices1, controller);
  switch2.Install (switchNode2, switchDevices2, controller);
  switch3.Install (switchNode3, switchDevices3, controller);


  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);
  
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer addresses=ipv4.Assign (terminalDevices);

  //creation of TCP traffic
  NS_LOG_INFO ("Create Applications.");

  //Create a BulkSendApplication and install it on node 0
  BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address("10.1.1.2"), 10));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source1.SetAttribute ("MaxBytes", UintegerValue (1));
  ApplicationContainer sourceApps1 = source1.Install (terminals.Get (0));
  sourceApps1.Start (Seconds (1.0));
  sourceApps1.Stop (Seconds (10.0));


  BulkSendHelper source2 ("ns3::TcpSocketFactory",
                           InetSocketAddress (Ipv4Address("10.1.1.2"), 2456));
    // Set the amount of data to send in bytes.  Zero is unlimited.
  source2.SetAttribute ("MaxBytes", UintegerValue (1));
  ApplicationContainer sourceApps2 = source2.Install (terminals.Get (0));
  sourceApps2.Start (Seconds (1.0));
  sourceApps2.Stop (Seconds (10.0));
  

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}
