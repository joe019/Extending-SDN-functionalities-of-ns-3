
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
  terminals.Create (2);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (7);

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices[7];
  // NetDeviceContainer terminalDevices2;
  NetDeviceContainer linkSwitch[7];
  for(int i=0;i<5;i++){
    linkSwitch[i] = csma.Install (NodeContainer (csmaSwitch.Get(i),csmaSwitch.Get(i+1)));
  }

      linkSwitch[5] = csma.Install (NodeContainer (csmaSwitch.Get(1),csmaSwitch.Get(6)));
      linkSwitch[6] = csma.Install (NodeContainer (csmaSwitch.Get(6),csmaSwitch.Get(4)));
  

  for(int i=0;i<5;i++){
    switchDevices[i].Add(linkSwitch[i].Get(0));
    switchDevices[i+1].Add(linkSwitch[i].Get(1));
  }
  
  switchDevices[1].Add(linkSwitch[5].Get(0));
  switchDevices[6].Add(linkSwitch[5].Get(1));

  switchDevices[6].Add(linkSwitch[6].Get(0));
  switchDevices[4].Add(linkSwitch[6].Get(1));

  
  NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (0), csmaSwitch.Get(0)));
  terminalDevices.Add (link.Get (0));
  switchDevices[0].Add (link.Get (1));
    
  link = csma.Install (NodeContainer (terminals.Get (1), csmaSwitch.Get(6)));
  terminalDevices.Add (link.Get (0));
  switchDevices[5].Add (link.Get (1));

    


  Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
   if (!timeout.IsZero ()) controller->SetAttribute ("ExpirationTime", TimeValue (timeout));
   

  Ptr<Node> switchNode;
  OpenFlowSwitchHelper ofSwitch[7];  
  for(int i=0;i<7;i++){
   switchNode = csmaSwitch.Get (0);
   
   ofSwitch[i].Install (switchNode, switchDevices[i], controller);
     
  }
 
  
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
  ipv4.Assign (terminalDevices);

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

   
NS_LOG_INFO ("Create Applications.");


//Create a BulkSendApplication and install it on node 0

  uint16_t port = 9;  // well-known echo port number

  //terminals.Get(0).Ipv4Address()
  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address("10.1.1.1"), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (10));
  ApplicationContainer sourceApps = source.Install (terminals.Get (1));
  sourceApps.Start (Seconds (1.0));
  sourceApps.Stop (Seconds (10.0));

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (terminals.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));

//UDP Socket

  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.1"), port)));
  onoff.SetConstantRate (DataRate ("5kb/s"));

  ApplicationContainer app1 = onoff.Install (terminals.Get (2));
  // Start the application
  app1.Start (Seconds (0.0));
  app1.Stop (Seconds (10.0));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  app1 = sink1.Install (terminals.Get (1));
  app1.Start (Seconds (0.0));











//    NS_LOG_INFO ("Configure Tracing.");

  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "openflow-switch.tr"
  //
  AsciiTraceHelper ascii;
   csma.EnableAsciiAll (ascii.CreateFileStream ("example1.tr"));

  //
  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     openflow-switch-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  //
   csma.EnablePcapAll ("example1", true);

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
