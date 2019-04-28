---
title: Implementation
layout: default
use_fontawesome: true
---

<h2>Proactive Topology</h2>
<p> A node learns the path to destination using the broadcast messages sent during ARP
request. This gives the controller very less capability to decide the path to be used
for packet traversal. To avoid this issue, we decided to make the controller aware
of the network topology ahead of packet transfer </p>
<h3>To do this:</h3>
  <ul>
<li>Whenever a switch is registered with the controller, details of its
neighbours are also learnt by the controller and minimum spanning
tree is generated.
<li>To get another path we can mark some of the switches as High traffic
switches and these can be accessed only by a particular traffic.
 </ul>
 <div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-8">
	    <img src="{{ site.baseurl }}/images/proactive.png" alt="proactive topology output">
    <p> This is the Screenshot when first packet transfered and reaches controller from first switch,<br> The details of all the end nodes and neighbour switches is already in the Table</p>
	</div>
	<div class="col-12 col-sm-2"></div>
</div>
<p> When the switch contacts the controller for the first time for the packet transfer,The topology is already known to the controller.So the controller adds the flow table entry in all the switches in the path of the packet.This reduces the overall time for the packet to reach the destination as no frequent request to the controller is required.<p> 
 <div class="row content-row">
	<div class="col-12 col-sm-2"></div>
	<div class="col-12 col-sm-8">
	    <img src="{{ site.baseurl }}/images/screenshot.png" alt="all switch one entry output">
    <p> This is the Screenshot we can see in a single controller call flow table entries are made in all the switches in the path of the packet</p>
	</div>
	<div class="col-12 col-sm-2"></div>
</div>
