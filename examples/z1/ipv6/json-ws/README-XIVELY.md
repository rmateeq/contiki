JSON ws
=======

Short description on how to set-up a sensor network for global IPv6 addresses.
NOTE: this assumes that you do not have a native IPv6 connection.

You will need:

- PC with Ubuntu (Linux) - 11 or 12 versions
- A node for the RPL-Border-Router (examples/ipv6/rpl-border-router)
- A node for the json webservice (examples/ipv6/json-ws)
- Existing IPv6 tunnel (tested with hurricane electric)

    Server IPv4 Address:216.66.80.30
    Server IPv6 Address:2001:470:1f0a:6b3::1/64
    Client IPv4 Address:213.151.115.180
    Client IPv6 Address:2001:470:1f0a:6b3::2/64

    Routed IPv6 Prefixes:
    Routed /64:2001:470:1f0b:6bc::/64
    Routed /48:2001:470:7279::/48

1. Connect one of the nodes to the PC (via USB or serial) and program
   it with the RPL-border-router (assumes Z1 node).

        cd contiki/examples/ipv6/rpl-border-router
        make DEFINES=DEFINES=NETSTACK_RDC=nullrdc_driver,NULLRDC_CONF_802154_AUTOACK=1 TARGET=z1 border-router.upload

2. Run tunslip6 which will forward IP from the RPL network to the IPv6 tunnel
   (and to the Internet).

        cd contiki/examples/ipv6/rpl-border-router
        make connect-router PREFIX=2001:470:1f0b:6bc::1/64

    When you start this you should get a printout from the border-router
    which give you the IPv6 address of it.

        Server IPv6 addresses:
        2001:470:1f0b:6bc:c30c::1
        fe80::c30c:0:0:1

3. Browse using Mozilla Firefox (or any other browser) to the IPv6 address
   given by the border router. This will show you the list of other nodes
   connected to the RPL network.

   http://[2001:470:1f0b:6bc:c30c::1]/

   NOTE: this is a global IPv6 address so it should also be reachable from
   any machine on the Internet.


Configuration of XIVELY submission
--------------------------------
1. Register a Xively account at https://xively.com/
   Set-up a feed and create an API key for the feed.

2. Program the sensor node with (assumes Z1)

   cd contiki/examples/ipv6/json-ws
   make websense-z1.upload WITH_XIVELY=1 TARGET=z1

3. Check the IPv6 address of the node via the RPL-border router or by looking
   at printouts when booting (make login TARGET=z1)

4. You need to configure the node to push data to the Xively feed and this can be
   done in several ways. For convenience a Python script is included that
   pushes the configuration to the nodes.

   Edit the file 'setxively.py' and replace "<your-key>" and "<your-feed>" with
   your Xively API key and Xively feed id. You can then use this Python script to
   configure your nodes.

   This is an example that configures the node with IP address
   2001:470:1f0b:6bc:c30c::3 to push data to the Xively feed with stream 1:

        cd contiki/examples/ipv6/json-ws
        ./setxively.py [2001:470:1f0b:6bc:c30c::3] Test

   Another way to configure the nodes is to use a REST add-on for the web
   browser to post a XIVELY configuration to the node. "REST Client" for Mozilla
   Firefox is an example of such add-on.

   POST a JSON expression to your node with the following data: This assumes
   that you have the feed with id 1868998277 and want to post to stream 1 in that
   feed. The field 'appdata' should be set to the API key you created at the
   Xively web site for the feed.

        {
         "host":"[2001:470:1f10:333::2]",
         "port":80,
         "path":"/v2/feeds/55180/datastreams/1",
         "appdata":"<insert your Xively API key>",
         "interval":120,
         "proto":"xively"
        }

   This will configure the node to periodically push temperature data every
   other minute. You can use GET to retrieve the data to se that the node has
   been successfully configured (the Xively API key will be visualized as a
   number of stars).
