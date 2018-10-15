#ifdef NS3_OPENFLOW

#include "openflow-switch-helper.h"
#include "ns3/log.h"
#include "ns3/openflow-switch-net-device.h"
#include "ns3/openflow-interface.h"
#include "ns3/node.h"
#include "ns3/names.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OpenFlowSwitchHelper");

OpenFlowSwitchHelper::OpenFlowSwitchHelper ()
{
  // Output the name of the function
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.SetTypeId ("ns3::OpenFlowSwitchNetDevice");
}

void
OpenFlowSwitchHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.Set (n1, v1);
}

// This method creates an ns3::OpenFlowSwitchNetDevice with the attributes configured by 
// OpenFlowSwitchHelper::SetDeviceAttribute, adds the device to the node, attaches the given 
// NetDevices as ports of the switch, and sets up a controller connection using the provided Controller 
NetDeviceContainer
OpenFlowSwitchHelper::Install (Ptr<Node> node, NetDeviceContainer c, Ptr<ns3::ofi::Controller> controller)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO ("**** Install switch device on node " << node->GetId ());

  NetDeviceContainer devs;
  Ptr<OpenFlowSwitchNetDevice> dev = m_deviceFactory.Create<OpenFlowSwitchNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);

  NS_LOG_INFO ("**** Set up Controller");
  dev->SetController (controller);

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_INFO ("**** Add SwitchPort " << *i);
      dev->AddSwitchPort (*i);
    }
  return devs;
}

NetDeviceContainer
OpenFlowSwitchHelper::Install (Ptr<Node> node, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO ("**** Install switch device on node " << node->GetId ());

  NetDeviceContainer devs;
  Ptr<OpenFlowSwitchNetDevice> dev = m_deviceFactory.Create<OpenFlowSwitchNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_INFO ("**** Add SwitchPort " << *i);
      dev->AddSwitchPort (*i);
    }
  return devs;
}

NetDeviceContainer
OpenFlowSwitchHelper::Install (std::string nodeName, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (node, c);
}

} // namespace ns3

#endif // NS3_OPENFLOW
