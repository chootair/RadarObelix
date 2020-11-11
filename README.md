# RadarObelix
Something cool around RADAR display

<img src="Doc/ScopeTrack.png" alt="drawing" width="400"/>


The aim of the radar OBELIX environment is to provide simple tools (message protocol + API) for displaying a radar situation.

The OBELIX protocol is widely inspired of the real [ASTERIX protocol](https://en.wikipedia.org/wiki/ASTERIX_(ATC_standard) "Wikipedia page") but in a very simple way. It defines two kind of messages Video and Track. Video messages allow to paint a PPI (Plane Plot Indicator) and Track messages contains track properties. The messages are sent over unicast UDP.

The OBELIX library includes different Qt/C++ classes for playing with the protocol. A message simulator (generator and transmitter), a message receiver and a plotter to paint the PPI and track symbols
