---
title: Implementation
layout: default
use_fontawesome: true
---
<h2>Steps to reproduce</h2>
The following steps have to be used to recreate the output.
<ul>
	<li>Instal ns-3 and enable OpenFlow integration</li>
	<p>Check out this <a href="https://github.com/joe019/Extending-SDN-functionality-in-ns-3/wiki/5.-ns-3">link</a>!</p>
	<li>Clone the repository</li>
	<p><code>git clone https://github.com/joe019/Extending-SDN-functionality-in-ns-3.git</code><br></p>
	<li>Copy contents</li>
	<p>Go to your ns3 workspace and replace the folder <code>"src/openflow"</code> with <code>"Extending-SDN-functionality-in-ns-3/openflow/"</code> from the cloned repository.</p>
	<li>Run an example</li>
	<p><code>./waf --run \"openflow-switch --verbose\"</code><br></p>
	<p>Any other example can be run by replacing "openflow-switch" with the example name.</p>
</ul>

<h2>Proactive learning</h2>
<p> A packet learns the path to its destination using broadcast messages sent during ARP
requests. This gives the controller very less capability to decide the path to be used
for packet traversal. To avoid this issue, we decided to make the controller aware
of the network topology ahead of packet transfer. </p>
<h3>What we did:</h3>
<ul>
	<li>Whenever a switch is registered with the controller, details of its
	neighbours are also learnt by the controller and minimum spanning
	tree is generated.</li>
	<li>To distinguish flows, switches can be marked as "high traffic" switches and these can be accessed only by a particular traffic.</li>
</ul>

<div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-6">
		<img src="{{ site.baseurl }}/images/proactive.png" alt="proactive topology output">
	    <p> <i>When call to controller is made from a switch for the first time,<br> the details of all the neighbouring nodes and switches are already in the table at the controller.</i> </p>
	</div>
	<div class="col-12 col-sm-4"></div>
</div>

<p> When a switch contacts a controller for the first time for a packet's transfer, the topology is already known to the controller. So the controller adds flow table entry in all the switches in the path of the packet. This reduces the overall time for the packet to reach the destination, as no frequent requests to the controller are required.<p> 

<div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-6">
		<img src="{{ site.baseurl }}/images/screenshot.png" alt="all switch one entry output">
	    <p> Flow table entries are made in all the switches in the path of the packet in the first controller call.</p>
	</div>
	<div class="col-12 col-sm-4"></div>
</div>