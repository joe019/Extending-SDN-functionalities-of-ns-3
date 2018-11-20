---
title: SDN Simulation
---

# ns-3
## Download ns-3

If mercurial is installed on your system, you can download ns-3 directly:
```
hg clone http://code.nsnam.org/ns-3-dev
```

To obtain additional related libraries such as support for Python bindings, clone the ‘ns-3-allinone’ repository as follows, and run the download.py program:
```
hg clone http://code.nsnam.org/ns-3-allinone
cd ns-3-allinone && ./download.py
```

## Configuration
Change the current directory to ns-3-allinone and run the following: 
```
./build.py 
cd ns-3-dev
./waf configure --enable-examples --enable-tests
```

To run a sample program: 
```
./waf --run  first
```
 
## Openflow:

In order to use the OpenFlowSwitch module, you must create and link the OFSID (OpenFlow Software Implementation Distribution) to ns-3. To do this:

Obtain the OFSID code. An ns-3 specific OFSID branch is provided to ensure operation with ns-3. Use mercurial to download this branch and waf to build the library:
```
hg clone http://code.nsnam.org/openflow
cd openflow
```

From the “openflow” directory, run:
```
./waf configure
./waf build
```

Your OFSID is now built into a libopenflow.a library! To link to an ns-3 build with this OpenFlow switch module, run from the ns-3-dev:
```
./waf configure --enable-examples --enable-tests --with-openflow=pathToOpenflow
where pathToOpenflow=/home/gian/openflow or /home/joe019/7th\ sem/projects/openflow
```

Under ---- Summary of optional NS-3 features: you should see:
```
"NS-3 OpenFlow Integration     : enabled"
```

indicating the library has been linked to ns-3. Run:
```
./waf build
```

to build ns-3 and activate the OpenFlowSwitch module in ns-3.

For an example demonstrating its use in a simple learning controller/switch, run:
```
./waf --run openflow-switch
```

To see it in detailed logging, run:
```
./waf --run "openflow-switch -v"
```

## References      
[Developer FAQs](https://www.nsnam.org/wiki/Developer_FAQ)<br>
[User FAQs](https://www.nsnam.org/wiki/User_FAQ)<br>
[OpenFlow Switch Device](https://www.nsnam.org/doxygen/group__openflow.html)<br>
[OpenFlow switch support](https://www.nsnam.org/docs/release/3.29/models/html/openflow-switch.html)
