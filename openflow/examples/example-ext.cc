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
      LogComponentEnable ("OpenFlowSwitchHelper", LOG_LEVEL_INFO);
    }

  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (2);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (5);


  /*NodeContainer csmaSwitchAddnal;
    csmaSwitchAddnal.Create (2);

*/

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;

  NetDeviceContainer switchDevices1;
  NetDeviceContainer switchDevices2;
  NetDeviceContainer switchDevices3;
  NetDeviceContainer switchDevices4;
  NetDeviceContainer switchDevices5;

  OpenFlowSwitchHelper swtch1;
  OpenFlowSwitchHelper swtch2;
  OpenFlowSwitchHelper swtch3;
  OpenFlowSwitchHelper swtch4;
  OpenFlowSwitchHelper swtch5;

  NetDeviceContainer linkSwitchA = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(1)));
  NetDeviceContainer linkSwitchB = csma.Install (NodeContainer (csmaSwitch.Get(1),csmaSwitch.Get(2)));
  //NetDeviceContainer linkSwitchC = csma.Install (NodeContainer (csmaSwitch.Get(2),csmaSwitch.Get(0)));//making straight topology


  NetDeviceContainer linkSwitchD = csma.Install (NodeContainer (csmaSwitch.Get(0),csmaSwitch.Get(3)));
  NetDeviceContainer linkSwitchE = csma.Install (NodeContainer (csmaSwitch.Get(3),csmaSwitch.Get(4)));
  NetDeviceContainer linkSwitchF = csma.Install (NodeContainer (csmaSwitch.Get(4),csmaSwitch.Get(2)));


  //switchDevices1.Add (linkSwitchA.Get (0));
  switchDevices1=swtch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(1)->GetDevice(0),linkSwitchA.Get(0));
  //switchDevices1.Add (linkSwitchC.Get (1));
  //switchDevices1=swtch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(2)->GetDevice(0),linkSwitchC.Get(1));
  //Additional path sw1
     switchDevices1=swtch1.addDeviceSwitch(switchDevices1,csmaSwitch.Get(3)->GetDevice(0),linkSwitchD.Get(0));




 // switchDevices2.Add (linkSwitchA.Get (1));
  switchDevices2=swtch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(0)->GetDevice(0),linkSwitchA.Get(1));
 // switchDevices2.Add (linkSwitchB.Get (0));
  switchDevices2=swtch2.addDeviceSwitch(switchDevices2,csmaSwitch.Get(2)->GetDevice(0),linkSwitchB.Get(0));

  //switchDevices3.Add (linkSwitchB.Get (1));
  switchDevices3=swtch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(1)->GetDevice(0),linkSwitchB.Get(1));
  //switchDevices3.Add (linkSwitchC.Get (0));
 // switchDevices3=swtch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(0)->GetDevice(0),linkSwitchC.Get(0));
  //Additional path sw3
   switchDevices3=swtch3.addDeviceSwitch(switchDevices3,csmaSwitch.Get(4)->GetDevice(0),linkSwitchF.Get(1));




  //Additional path

    switchDevices4=swtch4.addDeviceSwitch(switchDevices4,csmaSwitch.Get(0)->GetDevice(0),linkSwitchD.Get(1));
    switchDevices4=swtch4.addDeviceSwitch(switchDevices4,csmaSwitch.Get(4)->GetDevice(0),linkSwitchE.Get(0));

    switchDevices5=swtch5.addDeviceSwitch(switchDevices5,csmaSwitch.Get(3)->GetDevice(0),linkSwitchE.Get(1));
    switchDevices5=swtch5.addDeviceSwitch(switchDevices5,csmaSwitch.Get(2)->GetDevice(0),linkSwitchF.Get(0));



  NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (0), csmaSwitch.Get(0)));
  terminalDevices.Add (link.Get (0));
  //switchDevices1.Add (link.Get (1));
  switchDevices1=swtch1.addDeviceNode(switchDevices1,terminals.Get (0)->GetDevice(0),link.Get(1));

 // NS_LOG_INFO ("device_added"<<Mac48Address::ConvertFrom(((link.Get(1))->GetDevice(0))->GetAddress()));


  link = csma.Install (NodeContainer (terminals.Get (1), csmaSwitch.Get(2)));
  terminalDevices.Add (link.Get (0));
 // switchDevices3.Add (link.Get (1));
  switchDevices3=swtch3.addDeviceNode(switchDevices3,terminals.Get (1)->GetDevice(0),link.Get(1));


swtch4.toggleTrafficFlag();
swtch5.toggleTrafficFlag();



  //create controller
  Ptr<ns3::ofi::LearningController> controllerA = CreateObject<ns3::ofi::LearningController> ();

 //create openflow switch
  Ptr<Node> switchNode1 = csmaSwitch.Get (0);
  

  Ptr<Node> switchNode2 = csmaSwitch.Get (1);


  Ptr<Node> switchNode3 = csmaSwitch.Get (2);

  Ptr<Node> switchNode4 = csmaSwitch.Get (3);

  Ptr<Node> switchNode5 = csmaSwitch.Get (4);

  swtch1.Install (switchNode1, switchDevices1, controllerA);

  swtch2.Install (switchNode2, switchDevices2, controllerA);
  swtch3.Install (switchNode3, switchDevices3, controllerA);

  swtch4.Install (switchNode4, switchDevices4, controllerA);

  swtch5.Install (switchNode5, switchDevices5, controllerA);


  InternetStackHelper internet;
  internet.Install (terminals);
  
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer addresses=ipv4.Assign (terminalDevices);





  //creation of TCP traffic

    NS_LOG_INFO ("Create Applications.");


 BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address("10.1.1.2"), 10));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (1));
  ApplicationContainer sourceApps = source.Install (terminals.Get (0));
  sourceApps.Start (Seconds (1.0));
  sourceApps.Stop (Seconds (100.0));



     OnOffHelper onoff ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), 2456)));
     onoff.SetConstantRate (DataRate ("5kb/s"));

     ApplicationContainer app = onoff.Install (terminals.Get (0));
      app.Start (Seconds (1.0));
      app.Stop (Seconds (30.0));


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}

