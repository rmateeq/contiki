# Tests for CEPT compliance testing (868MHz band)

## Applications

The tests are divided into two groups:

* Higher channel : 869.124908 MHz
* Lower channel  : 863 MHz

All applications as default uses the following settings:

`cc1200_802154g_863_870_fsk_50kbps`

Overrides the following:

````
/* Expected TX power (14dBm 0x7F) */
#define CC1200_EXPECTED_TXPOWER       0x7F
/* Expected RSSI offset */
#define CC1200_EXPECTED_RSSI_OFFSET  (-81)
````

These parameters are hard-coded in each test file, so each binary may be changed separately, altough this is not advisable

The test applications are organized as follow:

* `rx-cont-high`: continuous reception mode, higher channel
* `rx-cont-low`: continuous reception mode, lower channel
* `tx-cont-mod-high`: continuous transmission mode with modulation, higher channel
* `tx-cont-mod-low`: continuous transmission mode with modulation, lower channel
* `tx-cont-mod-switching-high`: continuous transmission mode with modulation, toogling the radio on/off every 2 seconds, higher channel
* `tx-cont-mod-switching-low`: continuous transmission mode with modulation, toogling the radio on/off every 2 seconds, lower channel
* `tx-cont-no-mod-high`: continuous transmission mode without modulation, higher channel
* `tx-cont-no-mod-low`: continuous transmission mode without modulation, lower channel

## Note

The `CC1200_CONTINUOUS_RX_TRANSPARENT` is set whenever we want to mimick the results obtained by the SmartRF Studio and the CC1200EM in Continuous RX mode, else the device is kept configured in packet mode, and just goes into RX.
