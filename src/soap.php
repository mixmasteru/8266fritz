<?php
$client = new SoapClient(null,array(
  "location"   => "http://fritz.box:49000/igdupnp/control/WANIPConn1",
  "uri"        => "urn:schemas-upnp-org:service:WANIPConnection:1",
  "soapaction" => "urn:schemas-upnp-org:service:WANIPConnection:1#GetExternalIPAddress",
  "noroot"     => True
));

$status = $client->GetExternalIPAddress();
var_dump($status);
