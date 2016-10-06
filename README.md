# 8266fritz
Up- Downstream hardware monitor for a FRITZ!box

It's possible to use a SOAP API on the FRITZ!box to get data about your internet connection.
E.g. you can query the up and downstream in bytes.

To test this soap call you can use a curl request:
```
curl -s "http://fritz.box:49000/igdupnp/control/WANCommonIFC1" -H "Content-Type: text/xml; charset="utf-8"" -H "SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetAddonInfos" -d "@linkspeed.xml"
```
This does a SOAP POST:
```
*   Trying 192.168.178.1...
* Connected to fritz.box (192.168.178.1) port 49000 (#0)
> POST /igdupnp/control/WANCommonIFC1 HTTP/1.1
> Host: fritz.box:49000
> User-Agent: curl/7.47.0
> Accept: */*
> Content-Type: text/xml; charset=utf-8
> SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetAddonInfos
> Content-Length: 324
>
* upload completely sent off: 324 out of 324 bytes
< HTTP/1.1 200 OK
< DATE: Tue, 04 Oct 2016 19:20:53 GMT
< SERVER: FRITZ!Box Fon WLAN 7390 UPnP/1.0 AVM FRITZ!Box Fon WLAN 7390 84.06.51
< CONNECTION: keep-alive
< CONTENT-LENGTH: 991
< CONTENT-TYPE: text/xml; charset="utf-8"
< EXT:
```

We can use this data to drive an analog [vu-meter](https://en.wikipedia.org/wiki/VU_meter) via the pulse-width modulation (PWM) on a analog ESP8266 GPIO. The meter only needs a small amount of amps, so we can drive this directly.
