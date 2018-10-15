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
// - CBR (Constant bitrate)/UDP flows from n0 to n1 and from n3 to n0
// - DropTail queues
// - Tracing of queues and packet receptions to file "openflow-switch.tr"
// - If order of adding nodes and netdevices is kept:
//      n0 = 00:00:00:00:00:01, n1 = 00:00:00:00:00:03, n3 = 00:00:00:00:00:07
//	and port number corresponds to node number, so port 0 is connected to n0, for example

#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/log.h"

// Every class exported by the ns3 library is enclosed in the ns3 namespace
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OpenFlowCsmaSwitchExample");

bool verbose = false;
bool use_drop = false;
ns3::Time timeout = ns3::Seconds (0);

bool SetVerbose (std::string value)
{
  verbose = true;
  return true;
}

bool SetDrop (std::string value)
{
  use_drop = true;
  return true;
}

bool SetTimeout (std::string value)
{
  try 
  {
  	  // atof -> Convert string to double; c_str -> Returns a pointer to an array that contains 
  	  // a null-terminated sequence of characters
      timeout = ns3::Seconds (atof (value.c_str ()));
      return true;
  }
  catch (...) 
  { 
   	  return false; 
  }
  return false;
}

int main (int argc, char *argv[])
{
  #ifdef NS3_OPENFLOW

  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
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

  // Explicitly create the nodes required by the topology (shown above)
  NS_LOG_INFO ("Create nodes.");
  // NodeContainer -> Keeps track of a set of node pointers
  NodeContainer terminals;
  // Create (n) -> Create n nodes and append pointers to them to the end of a NodeContainer
  terminals.Create (4);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);

  NS_LOG_INFO ("Build Topology");
  // CsmaHelper -> Build a set of CsmaNetDevice objects 
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices;
  for (int i = 0; i < 4; i++)
    {
      // Install -> This method creates an ns3::CsmaChannel with the attributes configured by 
      // CsmaHelper::SetChannelAttribute, an ns3::CsmaNetDevice with the attributes configured 
      // by CsmaHelper::SetDeviceAttribute and then adds the device to the node and attaches 
      // the channel to the device 
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
      // Get (i) -> Get the Ptr<Node> stored in a container at a given index i
      terminalDevices.Add (link.Get (0));
      // Add -> Append the contents of another NetDeviceContainer to the end of a container
      switchDevices.Add (link.Get (1));
    }

  // Create the switch netdevice, which will do the packet switching
  Ptr<Node> switchNode = csmaSwitch.Get (0);
  // OpenFlowSwitchHelper -> Add capability to switch multiple LAN segments
  OpenFlowSwitchHelper swtch;

  if (use_drop)
    {
      Ptr<ns3::ofi::DropController> controller = CreateObject<ns3::ofi::DropController> ();
      swtch.Install (switchNode, switchDevices, controller);
    }
  else
    {
      Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
      if (!timeout.IsZero ()) controller->SetAttribute ("ExpirationTime", TimeValue (timeout));
      // Install -> This method creates an ns3::OpenFlowSwitchNetDevice with the attributes configured by 
      // OpenFlowSwitchHelper::SetDeviceAttribute, adds the device to the node, attaches the given 
      // NetDevices as ports of the switch, and sets up a controller connection using the provided Controller 
      swtch.Install (switchNode, switchDevices, controller);
    }

  // Add internet stack to the terminals
  // InternetStackHelper -> Aggregate IP/TCP/UDP functionality to existing Nodes
  InternetStackHelper internet;
  internet.Install (terminals);

  // We've got the "hardware" in place; Now add IP addresses
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  // SetBase -> Set the base network number, network mask and base address
  // For example, if you want to use a /24 prefix with an initial network number of 192.168.1 
  // (corresponding to a mask of 255.255.255.0) and start allocating IP addresses out of that 
  // network beginning at 192.168.1.3, you would call SetBase ("192.168.1.0", "255.255.255.0", "0.0.0.3");
  // If you don't care about the initial address it defaults to "0.0.0.1" in which case you can simply use,
  // SetBase ("192.168.1.0", "255.255.255.0");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  // Assign -> Assign IP addresses to the net devices specified in the container based on the current 
  // network prefix and address base
  ipv4.Assign (terminalDevices);

  // Create an OnOff application to send UDP datagrams from n0 to n1
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)

  // ns3::OnOffApplication -> Generate traffic to a single destination according to an OnOff pattern.
  // After Application::StartApplication is called, "On" and "Off" states alternate. The duration of 
  // each of these states is determined with the onTime and the offTime random variables. During the 
  // "Off" state, no traffic is generated. During the "On" state, cbr traffic is generated, characterized 
  // by the specified "data rate" and "packet size".
  // InetSocketAddress ->  this class holds an Ipv4Address and a port number to form an ipv4 transport endpoint
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port)));
  onoff.SetConstantRate (DataRate ("500kb/s"));

  // ApplicationContainer -> holds a vector of ns3::Application pointers. 
  // Typically ns-3 Applications are installed on nodes using an Application helper. 
  // The helper Install method takes a NodeContainer which holds some number of Ptr<Node>. 
  ApplicationContainer app = onoff.Install (terminals.Get (0));
  // Start the application
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  app = sink.Install (terminals.Get (1));
  app.Start (Seconds (0.0));

  // Create a similar flow from n3 to n0, starting at time 1.1 seconds
  onoff.SetAttribute ("Remote",
                      AddressValue (InetSocketAddress (Ipv4Address ("10.1.1.1"), port)));
  app = onoff.Install (terminals.Get (3));
  app.Start (Seconds (1.1));
  app.Stop (Seconds (10.0));

  app = sink.Install (terminals.Get (0));
  app.Start (Seconds (0.0));

  NS_LOG_INFO ("Configure Tracing.");

  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "openflow-switch.tr"
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("openflow-switch.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     openflow-switch-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  csma.EnablePcapAll ("openflow-switch", false);

  // Now, do the actual simulation.
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
  #else
  NS_LOG_INFO ("NS-3 OpenFlow is not enabled. Cannot run simulation.");
  #endif // NS3_OPENFLOW
}
