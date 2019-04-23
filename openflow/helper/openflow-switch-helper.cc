/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Blake Hurd
 *
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
 *
 * Author: Blake Hurd  <naimorai@gmail.com>
 */
#ifdef NS3_OPENFLOW

#include "openflow-switch-helper.h"
#include "ns3/log.h"
#include "ns3/openflow-switch-net-device.h"
#include "ns3/openflow-interface.h"
#include "ns3/node.h"
#include "ns3/names.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OpenFlowSwitchHelper");


typedef std::map<uint32_t,Mac48Address>t_portmap;
OpenFlowSwitchHelper::OpenFlowSwitchHelper ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.SetTypeId ("ns3::OpenFlowSwitchNetDevice");
}

void
OpenFlowSwitchHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.Set (n1, v1);
}

NetDeviceContainer
OpenFlowSwitchHelper::Install (Ptr<Node> node, NetDeviceContainer c, Ptr<ns3::ofi::Controller> controller)
{
	controller->create_path(Mac48Address::ConvertFrom((node->GetDevice(0)->GetAddress())),switchlist,nodelist,high_traffic_flag);
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO ("**** Install switch device on node " << node->GetId ());

  NetDeviceContainer devs;
  Ptr<OpenFlowSwitchNetDevice> dev = m_deviceFactory.Create<OpenFlowSwitchNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);

  //NS_LOG_INFO (dev<<"ring ringa rosses"<<Mac48Address::ConvertFrom(dev->GetAddress()) );

  NS_LOG_INFO ("**** Set up Controller");
  dev->SetController (controller);

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_INFO ("**** Add SwitchPort " << *i);
      dev->AddSwitchPort (*i);
    }
  controller->addmap(dev,Mac48Address::ConvertFrom(dev->GetAddress()));
  //NS_LOG_INFO (dev<<"ring ringa rosses"<<Mac48Address::ConvertFrom(dev->GetAddress()) );
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
//function to add node to switch
NetDeviceContainer
OpenFlowSwitchHelper::addDeviceSwitch(NetDeviceContainer switchNetDevice,Ptr< NetDevice > otherEnd,Ptr< NetDevice > myEnd)
{
	uint32_t size=switchNetDevice.GetN();
	switchlist.insert(std::make_pair (size,Mac48Address::ConvertFrom(otherEnd->GetAddress())));
	switchNetDevice.Add(myEnd);
	return switchNetDevice;
}
NetDeviceContainer
OpenFlowSwitchHelper::addDeviceNode(NetDeviceContainer switchNetDevice,Ptr< NetDevice > otherEnd,Ptr< NetDevice > myEnd)
{
	uint32_t size=switchNetDevice.GetN();
	nodelist.insert(std::make_pair (size,Mac48Address::ConvertFrom(otherEnd->GetAddress())));
	switchNetDevice.Add(myEnd);
	return switchNetDevice;
}
void
OpenFlowSwitchHelper:: toggleTrafficFlag()
{
	if(high_traffic_flag==1)
	{
		high_traffic_flag=0;
	}
	else
	{
		high_traffic_flag=1;
	}
}

} // namespace ns3

#endif // NS3_OPENFLOW
