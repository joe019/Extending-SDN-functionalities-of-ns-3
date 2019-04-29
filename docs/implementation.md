---
title: Implementation
layout: default
use_fontawesome: true
---
<h2>Steps to reproduce</h2>
The following steps have to be used to recreate the output.
1. Installing ns-3 and enabling OpenFlow integration
<p>Check out this <a href="https://github.com/joe019/Extending-SDN-functionality-in-ns-3/wiki/5.-ns-3">link</a>!<br></p>
2. Clone the repository <br>
<p><code>git clone https://github.com/joe019/Extending-SDN-functionality-in-ns-3.git</code><br></p>
3. Copy contents <br>
<p>Goto your ns3 workspace and replace the folder <code>"src/openflow"</code><br> with <code>"Extending-SDN-functionality-in-ns-3/openflow/"</code><br> from the cloned repository.</p>
4. Run an example <br>
<p><code>./waf --run \"openflow-switch --verbose\"</code><br></p>
<p>Any other example can be run by replacing "openflow-switch" with the example name.</p>


<h2>Proactive Topology</h2>
<p> A node learns the path to destination using the broadcast messages sent during ARP
request. This gives the controller very less capability to decide the path to be used
for packet traversal. To avoid this issue, we decided to make the controller aware
of the network topology ahead of packet transfer </p>
<h3>To do this:</h3>
<ul>
<li>Whenever a switch is registered with the controller, details of its
neighbours are also learnt by the controller and minimum spanning
tree is generated.</li>
<li>To get another path we can mark some of the switches as High traffic
switches and these can be accessed only by a particular traffic.</li>
</ul>
 <div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-8">
	    <img src="{{ site.baseurl }}/images/proactive.png" alt="proactive topology output">
    <p> This is the Screenshot which shows when controller call is made from first switch for first time,<br> The details of all the end nodes and neighbour switches is already in the table in controller. </p>
	</div>
	<div class="col-12 col-sm-2"></div>
</div>
<p> When the switch contacts the controller for the first time for the packet transfer,The topology is already known to the controller.So the controller adds the flow table entry in all the switches in the path of the packet.This reduces the overall time for the packet to reach the destination as no frequent request to the controller is required.<p> 
 <div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-8">
	    <img src="{{ site.baseurl }}/images/screenshot.png" alt="all switch one entry output">
    <p> In this the Screenshot we can see in a single controller call flow table entries are made in all the switches in the path of the packet.</p>
	</div>
	<div class="col-12 col-sm-2"></div>
</div>
