<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.5.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="13" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="50" name="dxf" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="53" name="tGND_GNDA" color="7" fill="9" visible="no" active="no"/>
<layer number="54" name="bGND_GNDA" color="1" fill="9" visible="no" active="no"/>
<layer number="56" name="wert" color="7" fill="1" visible="no" active="no"/>
<layer number="57" name="tCAD" color="7" fill="1" visible="no" active="no"/>
<layer number="59" name="tCarbon" color="7" fill="1" visible="no" active="no"/>
<layer number="60" name="bCarbon" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="7" fill="1" visible="yes" active="yes"/>
<layer number="100" name="Muster" color="7" fill="1" visible="no" active="no"/>
<layer number="101" name="Patch_Top" color="12" fill="4" visible="yes" active="yes"/>
<layer number="102" name="Vscore" color="7" fill="1" visible="yes" active="yes"/>
<layer number="103" name="tMap" color="7" fill="1" visible="yes" active="yes"/>
<layer number="104" name="Name" color="16" fill="1" visible="yes" active="yes"/>
<layer number="105" name="tPlate" color="7" fill="1" visible="yes" active="yes"/>
<layer number="106" name="bPlate" color="7" fill="1" visible="yes" active="yes"/>
<layer number="107" name="Crop" color="7" fill="1" visible="yes" active="yes"/>
<layer number="108" name="tplace-old" color="10" fill="1" visible="no" active="yes"/>
<layer number="109" name="ref-old" color="11" fill="1" visible="no" active="yes"/>
<layer number="110" name="fp0" color="7" fill="1" visible="yes" active="yes"/>
<layer number="111" name="LPC17xx" color="7" fill="1" visible="yes" active="yes"/>
<layer number="112" name="tSilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="113" name="IDFDebug" color="4" fill="1" visible="yes" active="yes"/>
<layer number="116" name="Patch_BOT" color="9" fill="4" visible="yes" active="yes"/>
<layer number="118" name="Rect_Pads" color="7" fill="1" visible="no" active="no"/>
<layer number="121" name="_tsilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="122" name="_bsilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="123" name="tTestmark" color="7" fill="1" visible="yes" active="yes"/>
<layer number="124" name="bTestmark" color="7" fill="1" visible="yes" active="yes"/>
<layer number="125" name="_tNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="126" name="_bNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="127" name="_tValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="128" name="_bValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="129" name="Mask" color="7" fill="1" visible="yes" active="yes"/>
<layer number="131" name="tAdjust" color="7" fill="1" visible="yes" active="yes"/>
<layer number="132" name="bAdjust" color="7" fill="1" visible="yes" active="yes"/>
<layer number="144" name="Drill_legend" color="7" fill="1" visible="yes" active="yes"/>
<layer number="150" name="Notes" color="7" fill="1" visible="yes" active="yes"/>
<layer number="151" name="HeatSink" color="7" fill="1" visible="yes" active="yes"/>
<layer number="152" name="_bDocu" color="7" fill="1" visible="yes" active="yes"/>
<layer number="153" name="FabDoc1" color="7" fill="1" visible="yes" active="yes"/>
<layer number="154" name="FabDoc2" color="7" fill="1" visible="yes" active="yes"/>
<layer number="155" name="FabDoc3" color="7" fill="1" visible="yes" active="yes"/>
<layer number="199" name="Contour" color="7" fill="1" visible="yes" active="yes"/>
<layer number="200" name="200bmp" color="1" fill="10" visible="yes" active="yes"/>
<layer number="201" name="201bmp" color="2" fill="10" visible="yes" active="yes"/>
<layer number="202" name="202bmp" color="3" fill="10" visible="yes" active="yes"/>
<layer number="203" name="203bmp" color="4" fill="10" visible="yes" active="yes"/>
<layer number="204" name="204bmp" color="5" fill="10" visible="yes" active="yes"/>
<layer number="205" name="205bmp" color="6" fill="10" visible="yes" active="yes"/>
<layer number="206" name="206bmp" color="7" fill="10" visible="yes" active="yes"/>
<layer number="207" name="207bmp" color="8" fill="10" visible="yes" active="yes"/>
<layer number="208" name="208bmp" color="9" fill="10" visible="yes" active="yes"/>
<layer number="209" name="209bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="210" name="210bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="211" name="211bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="212" name="212bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="213" name="213bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="214" name="214bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="215" name="215bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="216" name="216bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="217" name="217bmp" color="18" fill="1" visible="no" active="no"/>
<layer number="218" name="218bmp" color="19" fill="1" visible="no" active="no"/>
<layer number="219" name="219bmp" color="20" fill="1" visible="no" active="no"/>
<layer number="220" name="220bmp" color="21" fill="1" visible="no" active="no"/>
<layer number="221" name="221bmp" color="22" fill="1" visible="no" active="no"/>
<layer number="222" name="222bmp" color="23" fill="1" visible="no" active="no"/>
<layer number="223" name="223bmp" color="24" fill="1" visible="no" active="no"/>
<layer number="224" name="224bmp" color="25" fill="1" visible="no" active="no"/>
<layer number="225" name="225bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="226" name="226bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="227" name="227bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="228" name="228bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="229" name="229bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="230" name="230bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="231" name="Eagle3D_PG1" color="7" fill="1" visible="no" active="no"/>
<layer number="232" name="Eagle3D_PG2" color="7" fill="1" visible="no" active="no"/>
<layer number="233" name="Eagle3D_PG3" color="7" fill="1" visible="no" active="no"/>
<layer number="248" name="Housing" color="7" fill="1" visible="yes" active="yes"/>
<layer number="249" name="Edge" color="7" fill="1" visible="yes" active="yes"/>
<layer number="250" name="Descript" color="3" fill="1" visible="no" active="no"/>
<layer number="251" name="SMDround" color="12" fill="11" visible="no" active="no"/>
<layer number="254" name="cooling" color="7" fill="1" visible="yes" active="yes"/>
<layer number="255" name="routoute" color="7" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="Teensy3.0">
<packages>
<package name="TEENSY_3.0_BASIC_NO_BOTTOM_PINS">
<pad name="GND" x="-7.62" y="16.51" drill="0.9652"/>
<pad name="0" x="-7.62" y="13.97" drill="0.9652"/>
<pad name="1" x="-7.62" y="11.43" drill="0.9652"/>
<pad name="2" x="-7.62" y="8.89" drill="0.9652"/>
<pad name="3" x="-7.62" y="6.35" drill="0.9652"/>
<pad name="4" x="-7.62" y="3.81" drill="0.9652"/>
<pad name="5" x="-7.62" y="1.27" drill="0.9652"/>
<pad name="6" x="-7.62" y="-1.27" drill="0.9652"/>
<pad name="7" x="-7.62" y="-3.81" drill="0.9652"/>
<pad name="8" x="-7.62" y="-6.35" drill="0.9652"/>
<pad name="9" x="-7.62" y="-8.89" drill="0.9652"/>
<pad name="10" x="-7.62" y="-11.43" drill="0.9652"/>
<pad name="11" x="-7.62" y="-13.97" drill="0.9652"/>
<pad name="12" x="-7.62" y="-16.51" drill="0.9652"/>
<pad name="13" x="7.62" y="-16.51" drill="0.9652"/>
<pad name="14/A0" x="7.62" y="-13.97" drill="0.9652"/>
<pad name="15/A1" x="7.62" y="-11.43" drill="0.9652"/>
<pad name="16/A2" x="7.62" y="-8.89" drill="0.9652"/>
<pad name="17/A3" x="7.62" y="-6.35" drill="0.9652"/>
<pad name="18/A4" x="7.62" y="-3.81" drill="0.9652"/>
<pad name="19/A5" x="7.62" y="-1.27" drill="0.9652"/>
<pad name="20/A6" x="7.62" y="1.27" drill="0.9652"/>
<pad name="21/A7" x="7.62" y="3.81" drill="0.9652"/>
<pad name="22/A8" x="7.62" y="6.35" drill="0.9652"/>
<pad name="23/A9" x="7.62" y="8.89" drill="0.9652"/>
<pad name="3.3V" x="7.62" y="11.43" drill="0.9652"/>
<pad name="AGND" x="7.62" y="13.97" drill="0.9652"/>
<pad name="VIN" x="7.62" y="16.51" drill="0.9652"/>
<wire x1="-8.89" y1="17.78" x2="8.89" y2="17.78" width="0.127" layer="51"/>
<wire x1="8.89" y1="17.78" x2="8.89" y2="-17.78" width="0.127" layer="51"/>
<wire x1="8.89" y1="-17.78" x2="-8.89" y2="-17.78" width="0.127" layer="51"/>
<wire x1="-8.89" y1="-17.78" x2="-8.89" y2="17.78" width="0.127" layer="51"/>
<wire x1="-1.27" y1="16.51" x2="1.27" y2="16.51" width="0.2032" layer="21"/>
<wire x1="1.27" y1="16.51" x2="1.27" y2="17.78" width="0.2032" layer="21"/>
<wire x1="1.27" y1="17.78" x2="8.89" y2="17.78" width="0.2032" layer="21"/>
<wire x1="8.89" y1="17.78" x2="8.89" y2="-17.78" width="0.2032" layer="21"/>
<wire x1="8.89" y1="-17.78" x2="-8.89" y2="-17.78" width="0.2032" layer="21"/>
<wire x1="-8.89" y1="-17.78" x2="-8.89" y2="17.78" width="0.2032" layer="21"/>
<wire x1="-8.89" y1="17.78" x2="-1.27" y2="17.78" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="17.78" x2="-1.27" y2="16.51" width="0.2032" layer="21"/>
<text x="-2.54" y="13.97" size="1.27" layer="25" font="vector">&gt;NAME</text>
<text x="-3.81" y="-13.97" size="1.27" layer="27" font="vector">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="TEENSY_3.0_BASIC_NO_BOTTOM_PINS">
<wire x1="-10.16" y1="-22.86" x2="10.16" y2="-22.86" width="0.254" layer="94"/>
<wire x1="10.16" y1="-22.86" x2="10.16" y2="22.86" width="0.254" layer="94"/>
<wire x1="10.16" y1="22.86" x2="-10.16" y2="22.86" width="0.254" layer="94"/>
<wire x1="-10.16" y1="22.86" x2="-10.16" y2="-22.86" width="0.254" layer="94"/>
<pin name="12" x="-15.24" y="-17.78" visible="pin" length="middle"/>
<pin name="11" x="-15.24" y="-15.24" visible="pin" length="middle"/>
<pin name="10" x="-15.24" y="-12.7" visible="pin" length="middle"/>
<pin name="9" x="-15.24" y="-10.16" visible="pin" length="middle"/>
<pin name="8" x="-15.24" y="-7.62" visible="pin" length="middle"/>
<pin name="7" x="-15.24" y="-5.08" visible="pin" length="middle"/>
<pin name="6" x="-15.24" y="-2.54" visible="pin" length="middle"/>
<pin name="5" x="-15.24" y="0" visible="pin" length="middle"/>
<pin name="4" x="-15.24" y="2.54" visible="pin" length="middle"/>
<pin name="3" x="-15.24" y="5.08" visible="pin" length="middle"/>
<pin name="2" x="-15.24" y="7.62" visible="pin" length="middle"/>
<pin name="1" x="-15.24" y="10.16" visible="pin" length="middle"/>
<pin name="0" x="-15.24" y="12.7" visible="pin" length="middle"/>
<pin name="GND" x="15.24" y="10.16" visible="pin" length="middle" direction="pwr" rot="R180"/>
<pin name="VIN" x="15.24" y="15.24" visible="pin" length="middle" direction="pwr" rot="R180"/>
<pin name="AGND" x="15.24" y="7.62" visible="pin" length="middle" direction="pwr" rot="R180"/>
<pin name="3.3V" x="15.24" y="12.7" visible="pin" length="middle" direction="pwr" rot="R180"/>
<pin name="23/A9" x="15.24" y="2.54" visible="pin" length="middle" rot="R180"/>
<pin name="22/A8" x="15.24" y="0" visible="pin" length="middle" rot="R180"/>
<pin name="21/A7" x="15.24" y="-2.54" visible="pin" length="middle" rot="R180"/>
<pin name="20/A6" x="15.24" y="-5.08" visible="pin" length="middle" rot="R180"/>
<pin name="19/A5" x="15.24" y="-7.62" visible="pin" length="middle" rot="R180"/>
<pin name="18/A4" x="15.24" y="-10.16" visible="pin" length="middle" rot="R180"/>
<pin name="17/A3" x="15.24" y="-12.7" visible="pin" length="middle" rot="R180"/>
<pin name="16/A2" x="15.24" y="-15.24" visible="pin" length="middle" rot="R180"/>
<pin name="15/A1" x="15.24" y="-17.78" visible="pin" length="middle" rot="R180"/>
<pin name="14/A0" x="15.24" y="-20.32" visible="pin" length="middle" rot="R180"/>
<pin name="13" x="-15.24" y="-20.32" visible="pin" length="middle"/>
<text x="-3.048" y="24.13" size="1.27" layer="95" font="vector" ratio="15">&gt;NAME</text>
<text x="-2.794" y="-25.4" size="1.27" layer="96" font="vector" ratio="15">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="TEENSY_3.0_BASIC_NO_BOTTOM_PINS">
<description>Basic Teensy 3.0 with none of the 'inner' pins enabled. &lt;p&gt;See '+4', 'AllAnalogPins", and 'AllPins' packages for additional pin holes, as needed.</description>
<gates>
<gate name="G$1" symbol="TEENSY_3.0_BASIC_NO_BOTTOM_PINS" x="0" y="0"/>
</gates>
<devices>
<device name="" package="TEENSY_3.0_BASIC_NO_BOTTOM_PINS">
<connects>
<connect gate="G$1" pin="0" pad="0"/>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="11" pad="11"/>
<connect gate="G$1" pin="12" pad="12"/>
<connect gate="G$1" pin="13" pad="13"/>
<connect gate="G$1" pin="14/A0" pad="14/A0"/>
<connect gate="G$1" pin="15/A1" pad="15/A1"/>
<connect gate="G$1" pin="16/A2" pad="16/A2"/>
<connect gate="G$1" pin="17/A3" pad="17/A3"/>
<connect gate="G$1" pin="18/A4" pad="18/A4"/>
<connect gate="G$1" pin="19/A5" pad="19/A5"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="20/A6" pad="20/A6"/>
<connect gate="G$1" pin="21/A7" pad="21/A7"/>
<connect gate="G$1" pin="22/A8" pad="22/A8"/>
<connect gate="G$1" pin="23/A9" pad="23/A9"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="3.3V" pad="3.3V"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
<connect gate="G$1" pin="AGND" pad="AGND"/>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="SparkFun-Aesthetics">
<description>&lt;h3&gt;SparkFun Electronics' preferred foot prints&lt;/h3&gt;
In this library you'll find non-functional items- supply symbols, logos, notations, frame blocks, etc.&lt;br&gt;&lt;br&gt;
We've spent an enormous amount of time creating and checking these footprints and parts, but it is the end user's responsibility to ensure correctness and suitablity for a given componet or application. If you enjoy using this library, please buy one of our products at www.sparkfun.com.
&lt;br&gt;&lt;br&gt;
&lt;b&gt;Licensing:&lt;/b&gt; Creative Commons ShareAlike 4.0 International - https://creativecommons.org/licenses/by-sa/4.0/ 
&lt;br&gt;&lt;br&gt;
You are welcome to use this library for commercial purposes. For attribution, we ask that when you begin to sell your device using our footprint, you email us with a link to the product being sold. We want bragging rights that we helped (in a very small part) to create your 8th world wonder. We would like the opportunity to feature your device on our homepage.</description>
<packages>
</packages>
<symbols>
<symbol name="VIN">
<wire x1="0.762" y1="1.27" x2="0" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="-0.762" y2="1.27" width="0.254" layer="94"/>
<text x="-1.016" y="3.556" size="1.778" layer="96">&gt;VALUE</text>
<pin name="VIN" x="0" y="0" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="DGND">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="VIN" prefix="SUPPLY">
<description>Vin supply symbol</description>
<gates>
<gate name="G$1" symbol="VIN" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND" prefix="GND">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="DGND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="SparkFun-Connectors">
<description>&lt;h3&gt;SparkFun Electronics' preferred foot prints&lt;/h3&gt;
In this library you'll find connectors and sockets- basically anything that can be plugged into or onto.&lt;br&gt;&lt;br&gt;
We've spent an enormous amount of time creating and checking these footprints and parts, but it is the end user's responsibility to ensure correctness and suitablity for a given componet or application. If you enjoy using this library, please buy one of our products at www.sparkfun.com.
&lt;br&gt;&lt;br&gt;
&lt;b&gt;Licensing:&lt;/b&gt; Creative Commons ShareAlike 4.0 International - https://creativecommons.org/licenses/by-sa/4.0/ 
&lt;br&gt;&lt;br&gt;
You are welcome to use this library for commercial purposes. For attribution, we ask that when you begin to sell your device using our footprint, you email us with a link to the product being sold. We want bragging rights that we helped (in a very small part) to create your 8th world wonder. We would like the opportunity to feature your device on our homepage.</description>
<packages>
<package name="1X01_LONGPAD">
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="long" rot="R90"/>
<text x="-1.3462" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="LUXEON-PAD">
<smd name="P$1" x="0" y="0" dx="3.9" dy="2.4" layer="1" roundness="25"/>
<text x="-1.5" y="2" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.5" y="-3" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="SMTSO-256-ET">
<wire x1="-2.286" y1="1.016" x2="-1.016" y2="2.286" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="1.016" y1="2.286" x2="2.286" y2="1.016" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="2.286" y1="-1.016" x2="1.016" y2="-2.286" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="-1.016" y1="-2.286" x2="-2.286" y2="-1.016" width="1.016" layer="31" curve="-42.075022"/>
<circle x="0" y="0" radius="1.016" width="0.127" layer="51"/>
<pad name="P$1" x="0" y="0" drill="3.81" diameter="6.1976"/>
</package>
<package name="SMTRA-256-8-6">
<pad name="P$1" x="-1.9939" y="0" drill="1.3462"/>
<pad name="P$2" x="1.9939" y="0" drill="1.3462"/>
<smd name="P$3" x="0" y="0" dx="6.6548" dy="4.3434" layer="1" cream="no"/>
<text x="1.27" y="2.54" size="0.4064" layer="27">&gt;Value</text>
<text x="-2.54" y="2.54" size="0.4064" layer="25">&gt;Name</text>
<rectangle x1="-3.302" y1="0.762" x2="3.302" y2="2.032" layer="31"/>
<rectangle x1="-1.016" y1="0.508" x2="1.016" y2="0.762" layer="31"/>
<rectangle x1="-1.016" y1="-1.016" x2="1.016" y2="-0.762" layer="31"/>
<rectangle x1="-3.302" y1="-2.032" x2="3.302" y2="-0.762" layer="31"/>
<rectangle x1="-1.016" y1="-0.762" x2="1.016" y2="-0.508" layer="31"/>
<rectangle x1="2.794" y1="0.508" x2="3.302" y2="0.762" layer="31"/>
<rectangle x1="2.794" y1="-0.762" x2="3.302" y2="-0.508" layer="31"/>
<rectangle x1="-3.302" y1="-0.762" x2="-2.794" y2="-0.508" layer="31"/>
<rectangle x1="-3.302" y1="0.508" x2="-2.794" y2="0.762" layer="31"/>
</package>
<package name="1X01NS">
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
</package>
<package name="1X01">
<wire x1="1.27" y1="0.635" x2="0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="1.27" x2="-0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-1.27" y2="0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
<text x="1.1938" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="1X01_2MM">
<pad name="1" x="0" y="0" drill="2" diameter="3.302" rot="R90"/>
<text x="-1.3462" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="1X01_OFFSET">
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="offset" rot="R90"/>
<text x="1.1938" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="PAD-1.5X4.5">
<smd name="P$1" x="0" y="0" dx="1.5" dy="2" layer="1"/>
</package>
<package name="1X01_POGOPIN_HOLE_LARGE">
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.5494"/>
</package>
<package name="1X01_POGOPIN_HOLE_0.58">
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.4732"/>
</package>
<package name="SNAP-FEMALE">
<pad name="1" x="0" y="0" drill="2.921" diameter="4.572"/>
<polygon width="0.254" layer="1">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="29">
<vertex x="-4.0005" y="0" curve="-90.002865"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="31">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="41">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
</package>
<package name="SNAP-MALE">
<smd name="2" x="0" y="0" dx="7.62" dy="7.62" layer="1" roundness="100"/>
</package>
<package name="SPRING-CONNECTOR">
<smd name="P$2" x="0" y="0" dx="7.112" dy="7.112" layer="1" roundness="100"/>
<text x="-1.3462" y="5.6388" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.27" y="-5.715" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="1X01NS-KIT">
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90" stop="no"/>
<circle x="0" y="0" radius="0.508" width="0" layer="29"/>
<circle x="0" y="0" radius="0.9398" width="0" layer="30"/>
</package>
</packages>
<symbols>
<symbol name="M01">
<wire x1="3.81" y1="-2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<text x="-2.54" y="3.302" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="0" visible="off" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="M01" prefix="JP">
<description>&lt;b&gt;Header 1&lt;/b&gt;
Standard 1-pin 0.1" header. Use with straight break away headers (SKU : PRT-00116), right angle break away headers (PRT-00553), swiss pins (PRT-00743), machine pins (PRT-00117), and female headers (PRT-00115).</description>
<gates>
<gate name="G$1" symbol="M01" x="0" y="0"/>
</gates>
<devices>
<device name="PTH_LONGPAD" package="1X01_LONGPAD">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMD" package="LUXEON-PAD">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-SMTSO-256-ET" package="SMTSO-256-ET">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMTRA-256-8-6" package="SMTRA-256-8-6">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMDNS" package="1X01NS">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH" package="1X01">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH_2MM" package="1X01_2MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="OFFSET" package="1X01_OFFSET">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMD-4.5X1.5" package="PAD-1.5X4.5">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_LARGE" package="1X01_POGOPIN_HOLE_LARGE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_0.58" package="1X01_POGOPIN_HOLE_0.58">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-FEMALE" package="SNAP-FEMALE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-MALE" package="SNAP-MALE">
<connects>
<connect gate="G$1" pin="1" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SPRING-CONN" package="SPRING-CONNECTOR">
<connects>
<connect gate="G$1" pin="1" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="NOSILK-KIT" package="1X01NS-KIT">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="SparkFun">
<description>&lt;h3&gt;SparkFun Electronics' preferred foot prints&lt;/h3&gt;
We've spent an enormous amount of time creating and checking these footprints and parts. If you enjoy using this library, please buy one of our products at www.sparkfun.com.
&lt;br&gt;&lt;br&gt;
&lt;b&gt;Licensing:&lt;/b&gt; CC v3.0 Share-Alike You are welcome to use this library for commercial purposes. For attribution, we ask that when you begin to sell your device using our footprint, you email us with a link to the product being sold. We want bragging rights that we helped (in a very small part) to create your 8th world wonder. We would like the opportunity to feature your device on our homepage.</description>
<packages>
<package name="1206">
<wire x1="-2.473" y1="0.983" x2="2.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="-0.983" x2="-2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-2.473" y1="-0.983" x2="-2.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="0.983" x2="2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-0.965" y1="0.787" x2="0.965" y2="0.787" width="0.1016" layer="51"/>
<wire x1="-0.965" y1="-0.787" x2="0.965" y2="-0.787" width="0.1016" layer="51"/>
<smd name="1" x="-1.4" y="0" dx="1.6" dy="1.8" layer="1"/>
<smd name="2" x="1.4" y="0" dx="1.6" dy="1.8" layer="1"/>
<text x="-1.27" y="1.143" size="0.4064" layer="25">&gt;NAME</text>
<text x="-1.397" y="-1.524" size="0.4064" layer="27">&gt;VALUE</text>
<rectangle x1="-1.7018" y1="-0.8509" x2="-0.9517" y2="0.8491" layer="51"/>
<rectangle x1="0.9517" y1="-0.8491" x2="1.7018" y2="0.8509" layer="51"/>
<rectangle x1="-0.1999" y1="-0.4001" x2="0.1999" y2="0.4001" layer="35"/>
</package>
<package name="AXIAL-0.3">
<wire x1="-2.54" y1="0.762" x2="2.54" y2="0.762" width="0.2032" layer="21"/>
<wire x1="2.54" y1="0.762" x2="2.54" y2="0" width="0.2032" layer="21"/>
<wire x1="2.54" y1="0" x2="2.54" y2="-0.762" width="0.2032" layer="21"/>
<wire x1="2.54" y1="-0.762" x2="-2.54" y2="-0.762" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-0.762" x2="-2.54" y2="0" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="0" x2="-2.54" y2="0.762" width="0.2032" layer="21"/>
<wire x1="2.54" y1="0" x2="2.794" y2="0" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="0" x2="-2.794" y2="0" width="0.2032" layer="21"/>
<pad name="P$1" x="-3.81" y="0" drill="0.9" diameter="1.8796"/>
<pad name="P$2" x="3.81" y="0" drill="0.9" diameter="1.8796"/>
<text x="-2.54" y="1.27" size="0.4064" layer="25" font="vector">&gt;Name</text>
<text x="-2.032" y="-0.381" size="1.016" layer="21" font="vector" ratio="15">&gt;Value</text>
</package>
<package name="R2010">
<description>&lt;b&gt;RESISTOR&lt;/b&gt;&lt;p&gt;
chip</description>
<wire x1="-1.662" y1="1.245" x2="1.662" y2="1.245" width="0.1524" layer="51"/>
<wire x1="-1.637" y1="-1.245" x2="1.687" y2="-1.245" width="0.1524" layer="51"/>
<wire x1="-3.473" y1="1.483" x2="3.473" y2="1.483" width="0.0508" layer="39"/>
<wire x1="3.473" y1="1.483" x2="3.473" y2="-1.483" width="0.0508" layer="39"/>
<wire x1="3.473" y1="-1.483" x2="-3.473" y2="-1.483" width="0.0508" layer="39"/>
<wire x1="-3.473" y1="-1.483" x2="-3.473" y2="1.483" width="0.0508" layer="39"/>
<wire x1="-1.027" y1="1.245" x2="1.027" y2="1.245" width="0.1524" layer="21"/>
<wire x1="-1.002" y1="-1.245" x2="1.016" y2="-1.245" width="0.1524" layer="21"/>
<smd name="1" x="-2.2" y="0" dx="1.8" dy="2.7" layer="1"/>
<smd name="2" x="2.2" y="0" dx="1.8" dy="2.7" layer="1"/>
<text x="-2.54" y="1.5875" size="0.4064" layer="25">&gt;NAME</text>
<text x="-2.54" y="-2.032" size="0.4064" layer="27">&gt;VALUE</text>
<rectangle x1="-2.4892" y1="-1.3208" x2="-1.6393" y2="1.3292" layer="51"/>
<rectangle x1="1.651" y1="-1.3208" x2="2.5009" y2="1.3292" layer="51"/>
</package>
<package name="0805">
<wire x1="-0.3" y1="0.6" x2="0.3" y2="0.6" width="0.1524" layer="21"/>
<wire x1="-0.3" y1="-0.6" x2="0.3" y2="-0.6" width="0.1524" layer="21"/>
<smd name="1" x="-0.9" y="0" dx="0.8" dy="1.2" layer="1"/>
<smd name="2" x="0.9" y="0" dx="0.8" dy="1.2" layer="1"/>
<text x="-0.762" y="0.8255" size="0.4064" layer="25">&gt;NAME</text>
<text x="-1.016" y="-1.397" size="0.4064" layer="27">&gt;VALUE</text>
</package>
<package name="0603-RES">
<wire x1="-1.473" y1="0.983" x2="1.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="1.473" y1="0.983" x2="1.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="1.473" y1="-0.983" x2="-1.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-1.473" y1="-0.983" x2="-1.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="-0.356" y1="0.432" x2="0.356" y2="0.432" width="0.1016" layer="51"/>
<wire x1="-0.356" y1="-0.419" x2="0.356" y2="-0.419" width="0.1016" layer="51"/>
<smd name="1" x="-0.85" y="0" dx="1.1" dy="1" layer="1"/>
<smd name="2" x="0.85" y="0" dx="1.1" dy="1" layer="1"/>
<text x="-0.889" y="0.762" size="0.4064" layer="25" font="vector">&gt;NAME</text>
<text x="-1.016" y="-1.143" size="0.4064" layer="27" font="vector">&gt;VALUE</text>
<rectangle x1="-0.8382" y1="-0.4699" x2="-0.3381" y2="0.4801" layer="51"/>
<rectangle x1="0.3302" y1="-0.4699" x2="0.8303" y2="0.4801" layer="51"/>
<rectangle x1="-0.1999" y1="-0.3" x2="0.1999" y2="0.3" layer="35"/>
<rectangle x1="-0.2286" y1="-0.381" x2="0.2286" y2="0.381" layer="21"/>
</package>
<package name="0402-RES">
<description>&lt;b&gt;CAPACITOR&lt;/b&gt;&lt;p&gt;
chip</description>
<wire x1="-0.245" y1="0.224" x2="0.245" y2="0.224" width="0.1524" layer="51"/>
<wire x1="0.245" y1="-0.224" x2="-0.245" y2="-0.224" width="0.1524" layer="51"/>
<wire x1="-1.473" y1="0.483" x2="1.473" y2="0.483" width="0.0508" layer="39"/>
<wire x1="1.473" y1="0.483" x2="1.473" y2="-0.483" width="0.0508" layer="39"/>
<wire x1="1.473" y1="-0.483" x2="-1.473" y2="-0.483" width="0.0508" layer="39"/>
<wire x1="-1.473" y1="-0.483" x2="-1.473" y2="0.483" width="0.0508" layer="39"/>
<smd name="1" x="-0.65" y="0" dx="0.7" dy="0.9" layer="1"/>
<smd name="2" x="0.65" y="0" dx="0.7" dy="0.9" layer="1"/>
<text x="-0.889" y="0.6985" size="0.4064" layer="25">&gt;NAME</text>
<text x="-1.0795" y="-1.143" size="0.4064" layer="27">&gt;VALUE</text>
<rectangle x1="-0.554" y1="-0.3048" x2="-0.254" y2="0.2951" layer="51"/>
<rectangle x1="0.2588" y1="-0.3048" x2="0.5588" y2="0.2951" layer="51"/>
<rectangle x1="-0.1999" y1="-0.3" x2="0.1999" y2="0.3" layer="35"/>
<rectangle x1="-0.2032" y1="-0.3556" x2="0.2032" y2="0.3556" layer="21"/>
</package>
<package name="1/6W-RES">
<description>1/6W Thru-hole Resistor - *UNPROVEN*</description>
<wire x1="-1.55" y1="0.85" x2="-1.55" y2="-0.85" width="0.2032" layer="21"/>
<wire x1="-1.55" y1="-0.85" x2="1.55" y2="-0.85" width="0.2032" layer="21"/>
<wire x1="1.55" y1="-0.85" x2="1.55" y2="0.85" width="0.2032" layer="21"/>
<wire x1="1.55" y1="0.85" x2="-1.55" y2="0.85" width="0.2032" layer="21"/>
<pad name="1" x="-2.5" y="0" drill="0.762"/>
<pad name="2" x="2.5" y="0" drill="0.762"/>
<text x="-1.2662" y="0.9552" size="0.6096" layer="25">&gt;NAME</text>
<text x="-1.423" y="-0.4286" size="0.8128" layer="21" ratio="15">&gt;VALUE</text>
</package>
<package name="R2512">
<wire x1="-2.362" y1="1.473" x2="2.387" y2="1.473" width="0.1524" layer="51"/>
<wire x1="-2.362" y1="-1.473" x2="2.387" y2="-1.473" width="0.1524" layer="51"/>
<smd name="1" x="-2.8" y="0" dx="1.8" dy="3.2" layer="1"/>
<smd name="2" x="2.8" y="0" dx="1.8" dy="3.2" layer="1"/>
<text x="-2.54" y="1.905" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-3.2004" y1="-1.5494" x2="-2.3505" y2="1.5507" layer="51"/>
<rectangle x1="2.3622" y1="-1.5494" x2="3.2121" y2="1.5507" layer="51"/>
</package>
<package name="AXIAL-0.4">
<description>1/4W Resistor, 0.4" wide&lt;p&gt;

Yageo CFR series &lt;a href="http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf"&gt;http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf&lt;/a&gt;</description>
<wire x1="-3.15" y1="-1.2" x2="-3.15" y2="1.2" width="0.2032" layer="21"/>
<wire x1="-3.15" y1="1.2" x2="3.15" y2="1.2" width="0.2032" layer="21"/>
<wire x1="3.15" y1="1.2" x2="3.15" y2="-1.2" width="0.2032" layer="21"/>
<wire x1="3.15" y1="-1.2" x2="-3.15" y2="-1.2" width="0.2032" layer="21"/>
<pad name="P$1" x="-5.08" y="0" drill="0.9" diameter="1.8796"/>
<pad name="P$2" x="5.08" y="0" drill="0.9" diameter="1.8796"/>
<text x="-3.175" y="1.905" size="0.8128" layer="25" font="vector" ratio="15">&gt;Name</text>
<text x="-2.286" y="-0.381" size="0.8128" layer="21" font="vector" ratio="15">&gt;Value</text>
</package>
<package name="AXIAL-0.5">
<description>1/2W Resistor, 0.5" wide&lt;p&gt;

Yageo CFR series &lt;a href="http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf"&gt;http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf&lt;/a&gt;</description>
<wire x1="-4.5" y1="-1.65" x2="-4.5" y2="1.65" width="0.2032" layer="21"/>
<wire x1="-4.5" y1="1.65" x2="4.5" y2="1.65" width="0.2032" layer="21"/>
<wire x1="4.5" y1="1.65" x2="4.5" y2="-1.65" width="0.2032" layer="21"/>
<wire x1="4.5" y1="-1.65" x2="-4.5" y2="-1.65" width="0.2032" layer="21"/>
<pad name="P$1" x="-6.35" y="0" drill="0.9" diameter="1.8796"/>
<pad name="P$2" x="6.35" y="0" drill="0.9" diameter="1.8796"/>
<text x="-4.445" y="2.54" size="0.8128" layer="25" font="vector" ratio="15">&gt;Name</text>
<text x="-3.429" y="-0.381" size="0.8128" layer="21" font="vector" ratio="15">&gt;Value</text>
</package>
<package name="AXIAL-0.6">
<description>1W Resistor, 0.6" wide&lt;p&gt;

Yageo CFR series &lt;a href="http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf"&gt;http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf&lt;/a&gt;</description>
<wire x1="-5.75" y1="-2.25" x2="-5.75" y2="2.25" width="0.2032" layer="21"/>
<wire x1="-5.75" y1="2.25" x2="5.75" y2="2.25" width="0.2032" layer="21"/>
<wire x1="5.75" y1="2.25" x2="5.75" y2="-2.25" width="0.2032" layer="21"/>
<wire x1="5.75" y1="-2.25" x2="-5.75" y2="-2.25" width="0.2032" layer="21"/>
<pad name="P$1" x="-7.62" y="0" drill="1.2" diameter="1.8796"/>
<pad name="P$2" x="7.62" y="0" drill="1.2" diameter="1.8796"/>
<text x="-5.715" y="3.175" size="0.8128" layer="25" font="vector" ratio="15">&gt;Name</text>
<text x="-4.064" y="-0.381" size="0.8128" layer="21" font="vector" ratio="15">&gt;Value</text>
</package>
<package name="AXIAL-0.8">
<description>2W Resistor, 0.8" wide&lt;p&gt;

Yageo CFR series &lt;a href="http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf"&gt;http://www.yageo.com/pdf/yageo/Leaded-R_CFR_2008.pdf&lt;/a&gt;</description>
<wire x1="-7.75" y1="-2.5" x2="-7.75" y2="2.5" width="0.2032" layer="21"/>
<wire x1="-7.75" y1="2.5" x2="7.75" y2="2.5" width="0.2032" layer="21"/>
<wire x1="7.75" y1="2.5" x2="7.75" y2="-2.5" width="0.2032" layer="21"/>
<wire x1="7.75" y1="-2.5" x2="-7.75" y2="-2.5" width="0.2032" layer="21"/>
<pad name="P$1" x="-10.16" y="0" drill="1.2" diameter="1.8796"/>
<pad name="P$2" x="10.16" y="0" drill="1.2" diameter="1.8796"/>
<text x="-7.62" y="3.175" size="0.8128" layer="25" font="vector" ratio="15">&gt;Name</text>
<text x="-5.969" y="-0.381" size="0.8128" layer="21" font="vector" ratio="15">&gt;Value</text>
</package>
<package name="AXIAL-0.3-KIT">
<description>&lt;h3&gt;AXIAL-0.3-KIT&lt;/h3&gt;

Commonly used for 1/4W through-hole resistors. 0.3" pitch between holes.&lt;br&gt;
&lt;br&gt;

&lt;b&gt;Warning:&lt;/b&gt; This is the KIT version of the AXIAL-0.3 package. This package has a smaller diameter top stop mask, which doesn't cover the diameter of the pad. This means only the bottom side of the pads' copper will be exposed. You'll only be able to solder to the bottom side.</description>
<wire x1="-2.54" y1="1.27" x2="2.54" y2="1.27" width="0.254" layer="21"/>
<wire x1="2.54" y1="1.27" x2="2.54" y2="0" width="0.254" layer="21"/>
<wire x1="2.54" y1="0" x2="2.54" y2="-1.27" width="0.254" layer="21"/>
<wire x1="2.54" y1="-1.27" x2="-2.54" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-2.54" y1="-1.27" x2="-2.54" y2="0" width="0.254" layer="21"/>
<wire x1="-2.54" y1="0" x2="-2.54" y2="1.27" width="0.254" layer="21"/>
<wire x1="2.54" y1="0" x2="2.794" y2="0" width="0.254" layer="21"/>
<wire x1="-2.54" y1="0" x2="-2.794" y2="0" width="0.254" layer="21"/>
<pad name="P$1" x="-3.81" y="0" drill="1.016" diameter="2.032" stop="no"/>
<pad name="P$2" x="3.81" y="0" drill="1.016" diameter="2.032" stop="no"/>
<text x="-2.54" y="1.27" size="0.4064" layer="25" font="vector">&gt;Name</text>
<text x="-2.159" y="-0.762" size="1.27" layer="21" font="vector" ratio="15">&gt;Value</text>
<polygon width="0.127" layer="30">
<vertex x="3.8201" y="-0.9449" curve="-90"/>
<vertex x="2.8652" y="-0.0152" curve="-90.011749"/>
<vertex x="3.8176" y="0.9602" curve="-90"/>
<vertex x="4.7676" y="-0.0178" curve="-90.024193"/>
</polygon>
<polygon width="0.127" layer="29">
<vertex x="3.8176" y="-0.4369" curve="-90.012891"/>
<vertex x="3.3731" y="-0.0127" curve="-90"/>
<vertex x="3.8176" y="0.4546" curve="-90"/>
<vertex x="4.2595" y="-0.0025" curve="-90.012967"/>
</polygon>
<polygon width="0.127" layer="30">
<vertex x="-3.8075" y="-0.9525" curve="-90"/>
<vertex x="-4.7624" y="-0.0228" curve="-90.011749"/>
<vertex x="-3.81" y="0.9526" curve="-90"/>
<vertex x="-2.86" y="-0.0254" curve="-90.024193"/>
</polygon>
<polygon width="0.127" layer="29">
<vertex x="-3.81" y="-0.4445" curve="-90.012891"/>
<vertex x="-4.2545" y="-0.0203" curve="-90"/>
<vertex x="-3.81" y="0.447" curve="-90"/>
<vertex x="-3.3681" y="-0.0101" curve="-90.012967"/>
</polygon>
</package>
<package name="1X01_LONGPAD">
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="long" rot="R90"/>
<text x="-1.3462" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="LUXEON-PAD">
<smd name="P$1" x="0" y="0" dx="3.9" dy="2.4" layer="1" roundness="25"/>
<text x="-1.5" y="2" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.5" y="-3" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="SMTSO-256-ET">
<wire x1="-2.286" y1="1.016" x2="-1.016" y2="2.286" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="1.016" y1="2.286" x2="2.286" y2="1.016" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="2.286" y1="-1.016" x2="1.016" y2="-2.286" width="1.016" layer="31" curve="-42.075022"/>
<wire x1="-1.016" y1="-2.286" x2="-2.286" y2="-1.016" width="1.016" layer="31" curve="-42.075022"/>
<circle x="0" y="0" radius="1.016" width="0.127" layer="51"/>
<pad name="P$1" x="0" y="0" drill="3.81" diameter="6.1976"/>
</package>
<package name="SMTRA-256-8-6">
<pad name="P$1" x="-1.9939" y="0" drill="1.3462"/>
<pad name="P$2" x="1.9939" y="0" drill="1.3462"/>
<smd name="P$3" x="0" y="0" dx="6.6548" dy="4.3434" layer="1" cream="no"/>
<text x="1.27" y="2.54" size="0.4064" layer="27">&gt;Value</text>
<text x="-2.54" y="2.54" size="0.4064" layer="25">&gt;Name</text>
<rectangle x1="-3.302" y1="0.762" x2="3.302" y2="2.032" layer="31"/>
<rectangle x1="-1.016" y1="0.508" x2="1.016" y2="0.762" layer="31"/>
<rectangle x1="-1.016" y1="-1.016" x2="1.016" y2="-0.762" layer="31"/>
<rectangle x1="-3.302" y1="-2.032" x2="3.302" y2="-0.762" layer="31"/>
<rectangle x1="-1.016" y1="-0.762" x2="1.016" y2="-0.508" layer="31"/>
<rectangle x1="2.794" y1="0.508" x2="3.302" y2="0.762" layer="31"/>
<rectangle x1="2.794" y1="-0.762" x2="3.302" y2="-0.508" layer="31"/>
<rectangle x1="-3.302" y1="-0.762" x2="-2.794" y2="-0.508" layer="31"/>
<rectangle x1="-3.302" y1="0.508" x2="-2.794" y2="0.762" layer="31"/>
</package>
<package name="1X01NS">
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
</package>
<package name="1X01">
<wire x1="1.27" y1="0.635" x2="0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="1.27" x2="-0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-1.27" y2="0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
<text x="1.1938" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="1X01_2MM">
<pad name="1" x="0" y="0" drill="2" diameter="3.302" rot="R90"/>
<text x="-1.3462" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="1X01_OFFSET">
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="offset" rot="R90"/>
<text x="1.1938" y="1.8288" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="PAD-1.5X4.5">
<smd name="P$1" x="0" y="0" dx="1.5" dy="2" layer="1"/>
</package>
<package name="1X01_POGOPIN_HOLE_LARGE">
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.5494"/>
</package>
<package name="1X01_POGOPIN_HOLE_0.58">
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.4732"/>
</package>
<package name="SNAP-FEMALE">
<pad name="1" x="0" y="0" drill="2.921" diameter="4.572"/>
<polygon width="0.254" layer="1">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="29">
<vertex x="-4.0005" y="0" curve="-90.002865"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="31">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="41">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
</package>
<package name="SNAP-MALE">
<smd name="2" x="0" y="0" dx="7.62" dy="7.62" layer="1" roundness="100"/>
</package>
</packages>
<symbols>
<symbol name="RESISTOR">
<wire x1="-2.54" y1="0" x2="-2.159" y2="1.016" width="0.1524" layer="94"/>
<wire x1="-2.159" y1="1.016" x2="-1.524" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="-1.524" y1="-1.016" x2="-0.889" y2="1.016" width="0.1524" layer="94"/>
<wire x1="-0.889" y1="1.016" x2="-0.254" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="-0.254" y1="-1.016" x2="0.381" y2="1.016" width="0.1524" layer="94"/>
<wire x1="0.381" y1="1.016" x2="1.016" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="1.016" y1="-1.016" x2="1.651" y2="1.016" width="0.1524" layer="94"/>
<wire x1="1.651" y1="1.016" x2="2.286" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="2.286" y1="-1.016" x2="2.54" y2="0" width="0.1524" layer="94"/>
<text x="-3.81" y="1.4986" size="1.778" layer="95">&gt;NAME</text>
<text x="-3.81" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="M01">
<wire x1="3.81" y1="-2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<text x="-2.54" y="3.302" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="0" visible="off" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="RESISTOR" prefix="R" uservalue="yes">
<description>&lt;b&gt;Resistor&lt;/b&gt;
Basic schematic elements and footprints for 0603, 1206, and PTH resistors.</description>
<gates>
<gate name="G$1" symbol="RESISTOR" x="0" y="0"/>
</gates>
<devices>
<device name="1206" package="1206">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="AXIAL-0.3" package="AXIAL-0.3">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="2010" package="R2010">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="0805-RES" package="0805">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="0603-RES" package="0603-RES">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="0402-RES" package="0402-RES">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH-1/6W" package="1/6W-RES">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="2512" package="R2512">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH-1/4W" package="AXIAL-0.4">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH-1/2W" package="AXIAL-0.5">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH-1W" package="AXIAL-0.6">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH-2W" package="AXIAL-0.8">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="KIT" package="AXIAL-0.3-KIT">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="M01" prefix="JP">
<description>&lt;b&gt;Header 1&lt;/b&gt;
Standard 1-pin 0.1" header. Use with straight break away headers (SKU : PRT-00116), right angle break away headers (PRT-00553), swiss pins (PRT-00743), machine pins (PRT-00117), and female headers (PRT-00115).</description>
<gates>
<gate name="G$1" symbol="M01" x="0" y="0"/>
</gates>
<devices>
<device name="PTH_LONGPAD" package="1X01_LONGPAD">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMD" package="LUXEON-PAD">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-SMTSO-256-ET" package="SMTSO-256-ET">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMTRA-256-8-6" package="SMTRA-256-8-6">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMDNS" package="1X01NS">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH" package="1X01">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH_2MM" package="1X01_2MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="OFFSET" package="1X01_OFFSET">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMD-4.5X1.5" package="PAD-1.5X4.5">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_LARGE" package="1X01_POGOPIN_HOLE_LARGE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_0.58" package="1X01_POGOPIN_HOLE_0.58">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-FEMALE" package="SNAP-FEMALE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-MALE" package="SNAP-MALE">
<connects>
<connect gate="G$1" pin="1" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="LEDCyrWheel">
<packages>
<package name="POLOLU_5V_9A_VOLTAGE_REGULATOR_NO_HOLES">
<wire x1="15.24" y1="-10.16" x2="15.24" y2="10.16" width="0.127" layer="21"/>
<pad name="GND3" x="16.51" y="1.27" drill="0.9" shape="square"/>
<pad name="GND2" x="16.51" y="3.81" drill="0.9" shape="square"/>
<pad name="VIN2" x="16.51" y="-1.27" drill="0.9"/>
<pad name="VIN3" x="16.51" y="-3.81" drill="0.9"/>
<pad name="VIN" x="19.05" y="-2.54" drill="2.1844"/>
<pad name="GND" x="19.05" y="2.54" drill="2.1844" shape="square"/>
<text x="20.5994" y="6.4262" size="1.524" layer="21" rot="R180">GND</text>
<text x="20.0914" y="-5.6134" size="1.524" layer="21" rot="R180">VIN</text>
<wire x1="15.24" y1="10.16" x2="21.59" y2="10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="10.16" x2="21.59" y2="-10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="-10.16" x2="15.24" y2="-10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="-10.16" x2="-19.05" y2="-10.16" width="0.127" layer="51"/>
<wire x1="21.59" y1="10.16" x2="-19.05" y2="10.16" width="0.127" layer="51"/>
<wire x1="-19.05" y1="10.16" x2="-19.05" y2="-10.16" width="0.127" layer="51"/>
<text x="-8.89" y="-1.27" size="1.27" layer="51">Pololu 5V 9A
Voltage Regulator</text>
</package>
<package name="POLOLU_5V_9A_VOLTAGE_REGULATOR">
<wire x1="15.24" y1="-10.16" x2="15.24" y2="10.16" width="0.127" layer="21"/>
<pad name="GND3" x="16.51" y="1.27" drill="0.9" shape="square"/>
<pad name="GND2" x="16.51" y="3.81" drill="0.9" shape="square"/>
<pad name="VIN2" x="16.51" y="-1.27" drill="0.9"/>
<pad name="VIN3" x="16.51" y="-3.81" drill="0.9"/>
<pad name="VIN" x="19.05" y="-2.54" drill="2.1844"/>
<pad name="GND" x="19.05" y="2.54" drill="2.1844" shape="square"/>
<text x="17.2212" y="4.8006" size="1.524" layer="21" rot="R90">GND</text>
<text x="17.2466" y="-8.6614" size="1.524" layer="21" rot="R90">VIN</text>
<pad name="HOLE" x="19.05" y="7.62" drill="2.159"/>
<pad name="P$8" x="19.05" y="7.62" drill="2.159"/>
<pad name="HOLE2" x="19.05" y="-7.62" drill="2.159"/>
<wire x1="15.24" y1="10.16" x2="21.59" y2="10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="10.16" x2="21.59" y2="-10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="-10.16" x2="15.24" y2="-10.16" width="0.127" layer="21"/>
<wire x1="21.59" y1="-10.16" x2="-19.05" y2="-10.16" width="0.127" layer="51"/>
<wire x1="21.59" y1="10.16" x2="-19.05" y2="10.16" width="0.127" layer="51"/>
<wire x1="-19.05" y1="10.16" x2="-19.05" y2="-10.16" width="0.127" layer="51"/>
<text x="-8.89" y="-1.27" size="1.27" layer="51">Pololu 5V 9A
Voltage Regulator</text>
</package>
<package name="VIN_HOLE">
<pad name="VIN" x="0" y="-2.54" drill="2.1844"/>
<pad name="GND" x="0" y="2.54" drill="2.1844" shape="square"/>
</package>
<package name="MINIMU-9_V3_NO_HOLE">
<wire x1="-12.7" y1="-6.35" x2="-12.7" y2="6.35" width="0.4064" layer="21"/>
<wire x1="-12.7" y1="-6.35" x2="11.43" y2="-6.35" width="0.4064" layer="21"/>
<wire x1="-12.7" y1="6.35" x2="11.43" y2="6.35" width="0.4064" layer="21"/>
<wire x1="11.43" y1="-6.35" x2="11.43" y2="6.35" width="0.4064" layer="21"/>
<pad name="SCL" x="-11.43" y="5.08" drill="0.9652"/>
<pad name="SDA" x="-11.43" y="2.54" drill="0.9652"/>
<pad name="VIN" x="-11.43" y="-2.54" drill="0.9652"/>
<pad name="VDD" x="-11.43" y="-5.08" drill="0.9652"/>
<pad name="GND" x="-11.43" y="0" drill="0.9652" shape="square"/>
<text x="-10.16" y="-1.27" size="2.54" layer="21"> MinIMU-9 v3</text>
</package>
<package name="MINIMU-9_V3">
<wire x1="-11.43" y1="-6.35" x2="-11.43" y2="6.35" width="0.4064" layer="21"/>
<wire x1="-11.43" y1="-6.35" x2="12.7" y2="-6.35" width="0.4064" layer="21"/>
<wire x1="-11.43" y1="6.35" x2="12.7" y2="6.35" width="0.4064" layer="21"/>
<wire x1="12.7" y1="-6.35" x2="12.7" y2="6.35" width="0.4064" layer="21"/>
<pad name="SCL" x="-10.16" y="5.08" drill="0.9652"/>
<pad name="SDA" x="-10.16" y="2.54" drill="0.9652"/>
<pad name="VIN" x="-10.16" y="-2.54" drill="0.9652"/>
<pad name="VDD" x="-10.16" y="-5.08" drill="0.9652"/>
<pad name="GND" x="-10.16" y="0" drill="0.9652" shape="square"/>
<pad name="P$7" x="10.16" y="3.81" drill="2.1844"/>
<text x="-8.89" y="-1.27" size="2.54" layer="21"> MinIMU-9 v3</text>
</package>
<package name="TEENSY_SD_CARD">
<description>Teensy SD Card Adapter
&lt;a href="https://www.pjrc.com/teensy/sd_adaptor.html"&gt;PJRC SD Adapater&lt;/a&gt;</description>
<wire x1="-8.89" y1="6.604" x2="8.89" y2="6.604" width="0.127" layer="51"/>
<wire x1="8.89" y1="6.604" x2="8.89" y2="-6.604" width="0.127" layer="51"/>
<wire x1="8.89" y1="-6.604" x2="-8.89" y2="-6.604" width="0.127" layer="51"/>
<wire x1="-8.89" y1="-6.604" x2="-8.89" y2="6.604" width="0.127" layer="51"/>
<pad name="MISO" x="7.62" y="5.08" drill="0.9652"/>
<pad name="MOSI" x="7.62" y="2.54" drill="0.9652"/>
<pad name="SCLK" x="7.62" y="0" drill="0.9652"/>
<pad name="SS" x="7.62" y="-2.54" drill="0.9652"/>
<pad name="GND" x="7.62" y="-5.08" drill="0.9652"/>
<pad name="SW" x="-7.62" y="5.08" drill="0.9652"/>
<pad name="VIN" x="-7.62" y="-5.08" drill="0.9652"/>
<text x="-5.588" y="3.937" size="1.27" layer="21">SW</text>
<text x="-5.842" y="-5.715" size="1.27" layer="21">5V VIN</text>
<text x="1.397" y="4.064" size="1.27" layer="21">MISO</text>
<text x="1.397" y="1.778" size="1.27" layer="21">MOSI</text>
<text x="1.143" y="-0.635" size="1.27" layer="21">SCLK</text>
<text x="3.429" y="-3.048" size="1.27" layer="21">SS</text>
<text x="2.032" y="-5.715" size="1.27" layer="21">GND</text>
<wire x1="-8.89" y1="-6.731" x2="-8.89" y2="6.731" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="6.731" x2="8.89" y2="6.731" width="0.3048" layer="21"/>
<wire x1="8.89" y1="6.731" x2="8.89" y2="-6.731" width="0.3048" layer="21"/>
<wire x1="8.89" y1="-6.731" x2="6.223" y2="-6.731" width="0.3048" layer="21"/>
<wire x1="-6.223" y1="-6.731" x2="-8.89" y2="-6.731" width="0.3048" layer="21"/>
<wire x1="-6.223" y1="5.842" x2="-6.223" y2="-6.731" width="0.3048" layer="21"/>
<wire x1="-6.223" y1="-6.731" x2="6.223" y2="-6.731" width="0.3048" layer="21"/>
<wire x1="6.223" y1="-6.731" x2="6.223" y2="5.842" width="0.3048" layer="21"/>
<wire x1="6.223" y1="5.842" x2="-6.223" y2="5.842" width="0.3048" layer="21"/>
</package>
<package name="ADAFRUIT_BLE_FRIEND_SPI">
<wire x1="-16.4338" y1="-12.7762" x2="23.368" y2="-12.7" width="0.127" layer="51"/>
<wire x1="-16.4338" y1="-12.7762" x2="-16.4338" y2="13.3858" width="0.127" layer="51"/>
<wire x1="-16.383" y1="13.462" x2="23.368" y2="13.462" width="0.127" layer="51"/>
<wire x1="23.368" y1="-12.7" x2="23.368" y2="13.462" width="0.127" layer="51"/>
<pad name="VIN" x="-13.97" y="-9.779" drill="0.9652"/>
<pad name="GND" x="-13.97" y="-7.239" drill="0.9652"/>
<pad name="RST" x="-13.97" y="-4.699" drill="0.9652"/>
<pad name="DFU" x="-13.97" y="-2.159" drill="0.9652"/>
<pad name="IRQ" x="-13.97" y="0.381" drill="0.9652"/>
<pad name="CS" x="-13.97" y="2.921" drill="0.9652"/>
<pad name="MOSI" x="-13.97" y="5.461" drill="0.9652"/>
<pad name="MISO" x="-13.97" y="8.001" drill="0.9652"/>
<pad name="SCK" x="-13.97" y="10.541" drill="0.9652"/>
<text x="-16.1798" y="12.7" size="1.016" layer="21" rot="R270">SCK</text>
<text x="-15.0368" y="7.366" size="1.016" layer="21" rot="R90">MI</text>
<text x="-14.859" y="4.5212" size="1.016" layer="21" rot="R90">MO</text>
<text x="-14.859" y="2.1844" size="1.016" layer="21" rot="R90">CS</text>
<text x="-14.9352" y="-0.3048" size="0.8128" layer="21" rot="R90">IRQ</text>
<text x="-15.0114" y="-3.302" size="0.8128" layer="21" rot="R90">DFU</text>
<text x="-14.9606" y="-6.2484" size="0.8128" layer="21" rot="R90">RST</text>
<text x="-14.9352" y="-8.9916" size="1.016" layer="21" rot="R90">GD</text>
<text x="-14.8844" y="-12.1666" size="1.27" layer="21" rot="R90">VIN</text>
<wire x1="-12.7" y1="13.4366" x2="-12.7" y2="-12.7" width="0.127" layer="21"/>
<wire x1="-12.7" y1="-12.7" x2="-16.4846" y2="-12.7" width="0.127" layer="21"/>
<wire x1="-16.51" y1="-12.7" x2="-16.4846" y2="-12.7" width="0.127" layer="21"/>
<wire x1="-16.4846" y1="-12.7" x2="-16.4846" y2="13.4366" width="0.127" layer="21"/>
<wire x1="-16.4846" y1="13.4366" x2="-12.7" y2="13.4366" width="0.127" layer="21"/>
<wire x1="-12.7" y1="13.4366" x2="-12.7" y2="13.4874" width="0.127" layer="21"/>
<text x="-8.89" y="1.27" size="1.27" layer="51">Adafruit BlueFruit LE SPI Friend</text>
</package>
</packages>
<symbols>
<symbol name="POLOLU_5V_9A_VOLTAGE_REGULATOR">
<pin name="VIN" x="2.54" y="-2.54" visible="pin" length="middle" direction="pwr" rot="R180"/>
<pin name="GND" x="2.54" y="0" visible="pin" length="middle" direction="pwr" rot="R180"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-5.08" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-5.08" x2="-10.16" y2="-5.08" width="0.254" layer="94"/>
<wire x1="-10.16" y1="-5.08" x2="-10.16" y2="2.54" width="0.254" layer="94"/>
<wire x1="-10.16" y1="2.54" x2="-2.54" y2="2.54" width="0.254" layer="94"/>
<text x="-9.906" y="3.302" size="1.27" layer="94">&gt;NAME</text>
<text x="-9.906" y="-6.858" size="1.27" layer="94">&gt;VALUE</text>
</symbol>
<symbol name="VIN_GND_HOLE">
<pin name="VIN" x="-10.16" y="2.54" visible="pin" length="middle" direction="pwr"/>
<pin name="GND" x="-10.16" y="0" visible="pin" length="middle" direction="pwr"/>
<wire x1="-5.08" y1="5.08" x2="-5.08" y2="-2.54" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-2.54" x2="2.54" y2="-2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="5.08" width="0.254" layer="94"/>
<wire x1="2.54" y1="5.08" x2="-5.08" y2="5.08" width="0.254" layer="94"/>
</symbol>
<symbol name="MINIMU-9_V3">
<pin name="SCL" x="-12.7" y="15.24" visible="pin" length="middle"/>
<pin name="SDA" x="-12.7" y="12.7" visible="pin" length="middle"/>
<pin name="GND" x="-12.7" y="10.16" visible="pin" length="middle" direction="pwr"/>
<pin name="VIN" x="-12.7" y="7.62" visible="pin" length="middle"/>
<pin name="VDD" x="-12.7" y="5.08" visible="pin" length="middle"/>
<wire x1="-7.62" y1="15.24" x2="-7.62" y2="5.08" width="0.254" layer="94"/>
<wire x1="-7.62" y1="5.08" x2="12.7" y2="5.08" width="0.254" layer="94"/>
<wire x1="12.7" y1="5.08" x2="12.7" y2="15.24" width="0.254" layer="94"/>
<wire x1="12.7" y1="15.24" x2="-7.62" y2="15.24" width="0.254" layer="94"/>
<text x="-7.62" y="16.256" size="1.27" layer="94">&gt;NAME</text>
<text x="-7.62" y="2.54" size="1.27" layer="94">&gt;VALUE</text>
</symbol>
<symbol name="TEENSY_SD_CARD">
<wire x1="-12.7" y1="7.62" x2="-12.7" y2="-7.62" width="0.254" layer="94"/>
<wire x1="-12.7" y1="-7.62" x2="10.16" y2="-7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="-7.62" x2="10.16" y2="7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="7.62" x2="-12.7" y2="7.62" width="0.254" layer="94"/>
<pin name="SW" x="-17.78" y="5.08" visible="pin" length="middle"/>
<pin name="VIN_5V" x="-17.78" y="-5.08" visible="pin" length="middle" direction="pwr"/>
<pin name="MISO" x="15.24" y="5.08" visible="pin" length="middle" rot="R180"/>
<pin name="MOSI" x="15.24" y="2.54" visible="pin" length="middle" rot="R180"/>
<pin name="SCLK" x="15.24" y="0" visible="pin" length="middle" rot="R180"/>
<pin name="SS" x="15.24" y="-2.54" visible="pin" length="middle" rot="R180"/>
<pin name="GND" x="15.24" y="-5.08" visible="pin" length="middle" direction="pwr" rot="R180"/>
<text x="-12.446" y="8.382" size="1.27" layer="95">&gt;Name</text>
<text x="-12.192" y="-9.652" size="1.27" layer="97">&gt;Value</text>
</symbol>
<symbol name="ADAFRUIT_BLE_FRIEND_SPI">
<pin name="MISO" x="-2.54" y="-10.16" visible="pin" length="middle"/>
<pin name="MOSI" x="-2.54" y="-7.62" visible="pin" length="middle"/>
<pin name="CS" x="-2.54" y="-5.08" visible="pin" length="middle"/>
<pin name="IRQ" x="-2.54" y="-2.54" visible="pin" length="middle"/>
<pin name="DFU" x="-2.54" y="0" visible="pin" length="middle"/>
<pin name="RST" x="-2.54" y="2.54" visible="pin" length="middle"/>
<pin name="GND" x="-2.54" y="5.08" visible="pin" length="middle" direction="pwr"/>
<pin name="VIN" x="-2.54" y="7.62" visible="pin" length="middle" direction="pwr"/>
<wire x1="2.54" y1="10.16" x2="2.54" y2="-15.24" width="0.254" layer="94"/>
<wire x1="2.54" y1="-15.24" x2="30.48" y2="-15.24" width="0.254" layer="94"/>
<wire x1="30.48" y1="-15.24" x2="30.48" y2="10.16" width="0.254" layer="94"/>
<wire x1="30.48" y1="10.16" x2="2.54" y2="10.16" width="0.254" layer="94"/>
<text x="3.556" y="11.43" size="1.27" layer="95">&gt;NAME</text>
<text x="3.048" y="-18.034" size="1.27" layer="96">&gt;VALUE</text>
<pin name="SCK" x="-2.54" y="-12.7" visible="pin" length="middle"/>
<text x="15.24" y="-5.08" size="1.778" layer="97">AdaFruit
BLE Friend
SPI</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="POLOLU_5V_9A_VOLTAGE_REGULATOR">
<gates>
<gate name="G$1" symbol="POLOLU_5V_9A_VOLTAGE_REGULATOR" x="6.35" y="1.27"/>
</gates>
<devices>
<device name="DEFAULT" package="POLOLU_5V_9A_VOLTAGE_REGULATOR">
<connects>
<connect gate="G$1" pin="GND" pad="GND GND2 GND3"/>
<connect gate="G$1" pin="VIN" pad="VIN VIN2 VIN3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="NO_HOLES" package="POLOLU_5V_9A_VOLTAGE_REGULATOR_NO_HOLES">
<connects>
<connect gate="G$1" pin="GND" pad="GND GND2 GND3"/>
<connect gate="G$1" pin="VIN" pad="VIN VIN2 VIN3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="VIN_GND_HOLE">
<gates>
<gate name="G$1" symbol="VIN_GND_HOLE" x="0" y="0"/>
</gates>
<devices>
<device name="" package="VIN_HOLE">
<connects>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MINIMU-9_V3">
<description>POLOLU's MinIMU-9 v3</description>
<gates>
<gate name="G$1" symbol="MINIMU-9_V3" x="-2.54" y="-10.16"/>
</gates>
<devices>
<device name="DEFAULT" package="MINIMU-9_V3">
<connects>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="SCL" pad="SCL"/>
<connect gate="G$1" pin="SDA" pad="SDA"/>
<connect gate="G$1" pin="VDD" pad="VDD"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="NO_HOLE" package="MINIMU-9_V3_NO_HOLE">
<connects>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="SCL" pad="SCL"/>
<connect gate="G$1" pin="SDA" pad="SDA"/>
<connect gate="G$1" pin="VDD" pad="VDD"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="TEENSY_SD_CARD">
<description>TEENSY SD Card Aadapter</description>
<gates>
<gate name="G$1" symbol="TEENSY_SD_CARD" x="-15.24" y="12.7"/>
</gates>
<devices>
<device name="" package="TEENSY_SD_CARD">
<connects>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="MISO" pad="MISO"/>
<connect gate="G$1" pin="MOSI" pad="MOSI"/>
<connect gate="G$1" pin="SCLK" pad="SCLK"/>
<connect gate="G$1" pin="SS" pad="SS"/>
<connect gate="G$1" pin="SW" pad="SW"/>
<connect gate="G$1" pin="VIN_5V" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="ADAFRUIT_BLE_FRIEND_SPI">
<gates>
<gate name="G$1" symbol="ADAFRUIT_BLE_FRIEND_SPI" x="-12.7" y="2.54"/>
</gates>
<devices>
<device name="" package="ADAFRUIT_BLE_FRIEND_SPI">
<connects>
<connect gate="G$1" pin="CS" pad="CS"/>
<connect gate="G$1" pin="DFU" pad="DFU"/>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="IRQ" pad="IRQ"/>
<connect gate="G$1" pin="MISO" pad="MISO"/>
<connect gate="G$1" pin="MOSI" pad="MOSI"/>
<connect gate="G$1" pin="RST" pad="RST"/>
<connect gate="G$1" pin="SCK" pad="SCK"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="adafruit">
<packages>
<package name="DIL14">
<description>&lt;b&gt;Dual In Line Package&lt;/b&gt;</description>
<wire x1="8.89" y1="2.921" x2="-8.89" y2="2.921" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="-2.921" x2="8.89" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="8.89" y1="2.921" x2="8.89" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="2.921" x2="-8.89" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="-2.921" x2="-8.89" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="1.016" x2="-8.89" y2="-1.016" width="0.1524" layer="21" curve="-180"/>
<pad name="1" x="-7.62" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-5.08" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="7.62" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="7.62" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="0" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="5.08" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="5.08" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="0" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="-2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="-5.08" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="-7.62" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-9.271" y="-3.048" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-6.731" y="-0.635" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
</package>
<package name="DIL14-LARDO">
<wire x1="9.906" y1="2.921" x2="-9.906" y2="2.921" width="0.1524" layer="21"/>
<wire x1="-9.906" y1="-2.921" x2="9.906" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="9.906" y1="2.921" x2="9.906" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="-9.906" y1="2.921" x2="-9.906" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-9.906" y1="-2.921" x2="-9.906" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-9.906" y1="1.016" x2="-9.906" y2="-1.016" width="0.1524" layer="21" curve="-180"/>
<pad name="1" x="-7.62" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-5.08" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="7.62" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="7.62" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="0" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="5.08" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="5.08" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="0" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="-2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="-5.08" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="-7.62" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-9.271" y="-3.048" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-6.731" y="-0.635" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="74125">
<wire x1="5.08" y1="0" x2="-5.08" y2="5.08" width="0.4064" layer="94"/>
<wire x1="5.08" y1="0" x2="-5.08" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-5.08" y1="-5.08" x2="-5.08" y2="5.08" width="0.4064" layer="94"/>
<wire x1="0" y1="4.826" x2="0" y2="5.588" width="0.1524" layer="94"/>
<circle x="0" y="3.81" radius="1.016" width="0.1524" layer="94"/>
<text x="2.54" y="3.175" size="1.778" layer="95">&gt;NAME</text>
<text x="2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<pin name="OE" x="0" y="10.16" visible="pad" length="middle" direction="in" rot="R270"/>
<pin name="I" x="-10.16" y="0" visible="pad" length="middle" direction="in"/>
<pin name="O" x="10.16" y="0" visible="pad" length="middle" direction="hiz" rot="R180"/>
</symbol>
<symbol name="PWRN">
<text x="-0.635" y="-0.635" size="1.778" layer="95">&gt;NAME</text>
<text x="1.905" y="-7.62" size="1.27" layer="95" rot="R90">GND</text>
<text x="1.905" y="5.08" size="1.27" layer="95" rot="R90">VCC</text>
<pin name="GND" x="0" y="-10.16" visible="pad" direction="pwr" rot="R90"/>
<pin name="VCC" x="0" y="10.16" visible="pad" direction="pwr" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="74*125" prefix="IC">
<description>Quad bus &lt;b&gt;BUFFER&lt;/b&gt;, 3-state</description>
<gates>
<gate name="A" symbol="74125" x="17.78" y="0" swaplevel="1"/>
<gate name="B" symbol="74125" x="17.78" y="-17.78" swaplevel="1"/>
<gate name="C" symbol="74125" x="45.72" y="0" swaplevel="1"/>
<gate name="D" symbol="74125" x="45.72" y="-17.78" swaplevel="1"/>
<gate name="P" symbol="PWRN" x="-5.08" y="-7.62" addlevel="request"/>
</gates>
<devices>
<device name="N" package="DIL14">
<connects>
<connect gate="A" pin="I" pad="2"/>
<connect gate="A" pin="O" pad="3"/>
<connect gate="A" pin="OE" pad="1"/>
<connect gate="B" pin="I" pad="5"/>
<connect gate="B" pin="O" pad="6"/>
<connect gate="B" pin="OE" pad="4"/>
<connect gate="C" pin="I" pad="9"/>
<connect gate="C" pin="O" pad="8"/>
<connect gate="C" pin="OE" pad="10"/>
<connect gate="D" pin="I" pad="12"/>
<connect gate="D" pin="O" pad="11"/>
<connect gate="D" pin="OE" pad="13"/>
<connect gate="P" pin="GND" pad="7"/>
<connect gate="P" pin="VCC" pad="14"/>
</connects>
<technologies>
<technology name=""/>
<technology name="LS"/>
</technologies>
</device>
<device name="N-LARDO" package="DIL14-LARDO">
<connects>
<connect gate="A" pin="I" pad="2"/>
<connect gate="A" pin="O" pad="3"/>
<connect gate="A" pin="OE" pad="1"/>
<connect gate="B" pin="I" pad="5"/>
<connect gate="B" pin="O" pad="6"/>
<connect gate="B" pin="OE" pad="4"/>
<connect gate="C" pin="I" pad="9"/>
<connect gate="C" pin="O" pad="8"/>
<connect gate="C" pin="OE" pad="10"/>
<connect gate="D" pin="I" pad="12"/>
<connect gate="D" pin="O" pad="11"/>
<connect gate="D" pin="OE" pad="13"/>
<connect gate="P" pin="GND" pad="7"/>
<connect gate="P" pin="VCC" pad="14"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="Teensy3.0" deviceset="TEENSY_3.0_BASIC_NO_BOTTOM_PINS" device=""/>
<part name="SUPPLY1" library="SparkFun-Aesthetics" deviceset="VIN" device=""/>
<part name="GND1" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="R1" library="SparkFun" deviceset="RESISTOR" device="PTH-1/4W" value="18k"/>
<part name="R2" library="SparkFun" deviceset="RESISTOR" device="PTH-1/4W" value="10k"/>
<part name="GND2" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="JP3_7.2V" library="SparkFun" deviceset="M01" device="SMDNS" value="M01SMDNS">
<attribute name="VOLTAGE" value="7.2v"/>
</part>
<part name="LED_DATA" library="SparkFun" deviceset="M01" device="SMDNS" value="M01SMDNS"/>
<part name="LED_CLK" library="SparkFun" deviceset="M01" device="SMDNS" value="M01SMDNS"/>
<part name="U$2" library="LEDCyrWheel" deviceset="TEENSY_SD_CARD" device=""/>
<part name="SUPPLY2" library="SparkFun-Aesthetics" deviceset="VIN" device=""/>
<part name="GND3" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="R3" library="SparkFun" deviceset="RESISTOR" device="PTH-1/4W" value="4.7k"/>
<part name="R4" library="SparkFun" deviceset="RESISTOR" device="PTH-1/4W" value="4.7k"/>
<part name="U$3" library="LEDCyrWheel" deviceset="MINIMU-9_V3" device="NO_HOLE"/>
<part name="GND4" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="SUPPLY3" library="SparkFun-Aesthetics" deviceset="VIN" device=""/>
<part name="BUTTON_1" library="SparkFun-Connectors" deviceset="M01" device="NOSILK-KIT" value="M01NOSILK-KIT"/>
<part name="BUTTON_2" library="SparkFun-Connectors" deviceset="M01" device="NOSILK-KIT" value="M01NOSILK-KIT"/>
<part name="U$5" library="LEDCyrWheel" deviceset="POLOLU_5V_9A_VOLTAGE_REGULATOR" device="NO_HOLES"/>
<part name="U$6" library="LEDCyrWheel" deviceset="VIN_GND_HOLE" device=""/>
<part name="GND5" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="GND6" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="U$7" library="LEDCyrWheel" deviceset="ADAFRUIT_BLE_FRIEND_SPI" device=""/>
<part name="GND8" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="SUPPLY5" library="SparkFun-Aesthetics" deviceset="VIN" device=""/>
<part name="IC1" library="adafruit" deviceset="74*125" device="N" technology="LS"/>
<part name="SUPPLY4" library="SparkFun-Aesthetics" deviceset="VIN" device=""/>
<part name="GND7" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="GND9" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
<part name="GND10" library="SparkFun-Aesthetics" deviceset="GND" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="101.6" y="2.54" size="1.778" layer="91">screw terminal block: 2-pin, 5mm pitch, http://www.pololu.com/product/2440</text>
<text x="76.2" y="142.24" size="1.778" layer="91">this is the exit terminal block to carry the 5v to other parts</text>
<text x="165.1" y="96.52" size="1.778" layer="91">Voltage divider (down to 3.3v) to read lipo cell value</text>
<text x="104.14" y="10.16" size="1.778" layer="91">TODO::74HCT245 :: voltage regulator for signal output from LED strip pins to 3.3v instead of 5v!!!!!</text>
<text x="12.7" y="2.54" size="1.778" layer="91">This is the 74AHCT125 for 3.3v -&gt; 5v signals</text>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="104.14" y="96.52"/>
<instance part="SUPPLY1" gate="G$1" x="71.12" y="132.08"/>
<instance part="GND1" gate="1" x="134.62" y="104.14"/>
<instance part="R1" gate="G$1" x="180.34" y="91.44"/>
<instance part="R2" gate="G$1" x="205.74" y="91.44"/>
<instance part="GND2" gate="1" x="218.44" y="81.28"/>
<instance part="JP3_7.2V" gate="G$1" x="160.02" y="91.44">
<attribute name="VOLTAGE" x="167.64" y="91.44" size="1.778" layer="96"/>
</instance>
<instance part="LED_DATA" gate="G$1" x="20.32" y="114.3"/>
<instance part="LED_CLK" gate="G$1" x="10.16" y="86.36"/>
<instance part="U$2" gate="G$1" x="60.96" y="58.42"/>
<instance part="SUPPLY2" gate="G$1" x="35.56" y="53.34"/>
<instance part="GND3" gate="1" x="88.9" y="50.8"/>
<instance part="R3" gate="G$1" x="48.26" y="25.4"/>
<instance part="R4" gate="G$1" x="48.26" y="33.02"/>
<instance part="U$3" gate="G$1" x="86.36" y="12.7"/>
<instance part="GND4" gate="1" x="66.04" y="12.7"/>
<instance part="SUPPLY3" gate="G$1" x="35.56" y="38.1"/>
<instance part="BUTTON_1" gate="G$1" x="154.94" y="109.22" rot="R180"/>
<instance part="BUTTON_2" gate="G$1" x="154.94" y="121.92" rot="R180"/>
<instance part="U$5" gate="G$1" x="20.32" y="137.16"/>
<instance part="U$6" gate="G$1" x="101.6" y="137.16"/>
<instance part="GND5" gate="1" x="48.26" y="139.7"/>
<instance part="GND6" gate="1" x="83.82" y="134.62"/>
<instance part="U$7" gate="G$1" x="165.1" y="71.12"/>
<instance part="GND8" gate="1" x="142.24" y="78.74" rot="R180"/>
<instance part="SUPPLY5" gate="G$1" x="149.86" y="78.74"/>
<instance part="IC1" gate="A" x="50.8" y="114.3" rot="R180"/>
<instance part="IC1" gate="B" x="35.56" y="86.36" rot="R180"/>
<instance part="IC1" gate="P" x="15.24" y="15.24"/>
<instance part="SUPPLY4" gate="G$1" x="15.24" y="27.94"/>
<instance part="GND7" gate="1" x="15.24" y="0"/>
<instance part="GND9" gate="1" x="50.8" y="101.6"/>
<instance part="GND10" gate="1" x="35.56" y="73.66"/>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="2"/>
<wire x1="210.82" y1="91.44" x2="218.44" y2="91.44" width="0.1524" layer="91"/>
<wire x1="218.44" y1="91.44" x2="218.44" y2="83.82" width="0.1524" layer="91"/>
<pinref part="GND2" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="GND"/>
<wire x1="76.2" y1="53.34" x2="88.9" y2="53.34" width="0.1524" layer="91"/>
<pinref part="GND3" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="GND4" gate="1" pin="GND"/>
<wire x1="66.04" y1="15.24" x2="66.04" y2="22.86" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="GND"/>
<wire x1="66.04" y1="22.86" x2="73.66" y2="22.86" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="BUTTON_2" gate="G$1" pin="1"/>
<wire x1="147.32" y1="121.92" x2="129.54" y2="121.92" width="0.1524" layer="91"/>
<label x="134.62" y="121.92" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U$6" gate="G$1" pin="GND"/>
<pinref part="GND6" gate="1" pin="GND"/>
<wire x1="91.44" y1="137.16" x2="83.82" y2="137.16" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND5" gate="1" pin="GND"/>
<wire x1="48.26" y1="142.24" x2="33.02" y2="137.16" width="0.1524" layer="91"/>
<pinref part="U$5" gate="G$1" pin="GND"/>
<wire x1="22.86" y1="137.16" x2="30.48" y2="137.16" width="0.1524" layer="91"/>
<wire x1="30.48" y1="137.16" x2="33.02" y2="137.16" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$1" gate="G$1" pin="GND"/>
<pinref part="GND1" gate="1" pin="GND"/>
<wire x1="119.38" y1="106.68" x2="134.62" y2="106.68" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$7" gate="G$1" pin="GND"/>
<wire x1="162.56" y1="76.2" x2="142.24" y2="76.2" width="0.1524" layer="91"/>
<pinref part="GND8" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="IC1" gate="P" pin="GND"/>
<pinref part="GND7" gate="1" pin="GND"/>
<wire x1="15.24" y1="5.08" x2="15.24" y2="2.54" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND9" gate="1" pin="GND"/>
<pinref part="IC1" gate="A" pin="OE"/>
</segment>
<segment>
<pinref part="IC1" gate="B" pin="OE"/>
<pinref part="GND10" gate="1" pin="GND"/>
</segment>
</net>
<net name="VIN" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="VIN"/>
<pinref part="SUPPLY1" gate="G$1" pin="VIN"/>
<pinref part="U$5" gate="G$1" pin="VIN"/>
<wire x1="71.12" y1="129.54" x2="71.12" y2="132.08" width="0.1524" layer="91"/>
<wire x1="22.86" y1="134.62" x2="58.42" y2="134.62" width="0.1524" layer="91"/>
<wire x1="58.42" y1="134.62" x2="58.42" y2="129.54" width="0.1524" layer="91"/>
<wire x1="58.42" y1="129.54" x2="71.12" y2="129.54" width="0.1524" layer="91"/>
<junction x="71.12" y="129.54"/>
<pinref part="U$6" gate="G$1" pin="VIN"/>
<wire x1="91.44" y1="139.7" x2="76.2" y2="139.7" width="0.1524" layer="91"/>
<wire x1="76.2" y1="139.7" x2="76.2" y2="129.54" width="0.1524" layer="91"/>
<wire x1="76.2" y1="129.54" x2="71.12" y2="129.54" width="0.1524" layer="91"/>
<wire x1="119.38" y1="111.76" x2="119.38" y2="132.08" width="0.1524" layer="91"/>
<wire x1="119.38" y1="132.08" x2="116.84" y2="132.08" width="0.1524" layer="91"/>
<wire x1="116.84" y1="132.08" x2="116.84" y2="129.54" width="0.1524" layer="91"/>
<wire x1="116.84" y1="129.54" x2="76.2" y2="129.54" width="0.1524" layer="91"/>
<junction x="76.2" y="129.54"/>
</segment>
<segment>
<pinref part="SUPPLY2" gate="G$1" pin="VIN"/>
<pinref part="U$2" gate="G$1" pin="VIN_5V"/>
<wire x1="35.56" y1="53.34" x2="43.18" y2="53.34" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="VIN"/>
<wire x1="73.66" y1="20.32" x2="35.56" y2="20.32" width="0.1524" layer="91"/>
<pinref part="SUPPLY3" gate="G$1" pin="VIN"/>
<wire x1="35.56" y1="20.32" x2="35.56" y2="25.4" width="0.1524" layer="91"/>
<pinref part="R4" gate="G$1" pin="1"/>
<wire x1="35.56" y1="25.4" x2="35.56" y2="33.02" width="0.1524" layer="91"/>
<wire x1="35.56" y1="33.02" x2="35.56" y2="38.1" width="0.1524" layer="91"/>
<wire x1="35.56" y1="33.02" x2="43.18" y2="33.02" width="0.1524" layer="91"/>
<junction x="35.56" y="33.02"/>
<pinref part="R3" gate="G$1" pin="1"/>
<wire x1="43.18" y1="25.4" x2="35.56" y2="25.4" width="0.1524" layer="91"/>
<junction x="35.56" y="25.4"/>
</segment>
<segment>
<pinref part="SUPPLY5" gate="G$1" pin="VIN"/>
<pinref part="U$7" gate="G$1" pin="VIN"/>
<wire x1="149.86" y1="78.74" x2="162.56" y2="78.74" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SUPPLY4" gate="G$1" pin="VIN"/>
<pinref part="IC1" gate="P" pin="VCC"/>
<wire x1="15.24" y1="27.94" x2="15.24" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N_PIN17" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="2"/>
<pinref part="R2" gate="G$1" pin="1"/>
<wire x1="185.42" y1="91.44" x2="193.04" y2="91.44" width="0.1524" layer="91"/>
<wire x1="193.04" y1="91.44" x2="200.66" y2="91.44" width="0.1524" layer="91"/>
<wire x1="193.04" y1="91.44" x2="193.04" y2="83.82" width="0.1524" layer="91"/>
<junction x="193.04" y="91.44"/>
<pinref part="U$1" gate="G$1" pin="17/A3"/>
<wire x1="119.38" y1="83.82" x2="193.04" y2="83.82" width="0.1524" layer="91"/>
</segment>
</net>
<net name="VIN_7_2V" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="1"/>
<wire x1="167.64" y1="91.44" x2="175.26" y2="91.44" width="0.1524" layer="91"/>
<pinref part="JP3_7.2V" gate="G$1" pin="1"/>
</segment>
</net>
<net name="N_PIN_2_LED_STRIP" class="0">
<segment>
<pinref part="LED_DATA" gate="G$1" pin="1"/>
<pinref part="IC1" gate="A" pin="O"/>
<wire x1="40.64" y1="114.3" x2="27.94" y2="114.3" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="SS"/>
<wire x1="76.2" y1="55.88" x2="78.74" y2="55.88" width="0.1524" layer="91"/>
<wire x1="78.74" y1="55.88" x2="78.74" y2="83.82" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="10"/>
<wire x1="78.74" y1="83.82" x2="88.9" y2="83.82" width="0.1524" layer="91"/>
</segment>
</net>
<net name="MOSI" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="11"/>
<wire x1="88.9" y1="81.28" x2="81.28" y2="81.28" width="0.1524" layer="91"/>
<wire x1="81.28" y1="81.28" x2="81.28" y2="60.96" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="MOSI"/>
<wire x1="81.28" y1="60.96" x2="76.2" y2="60.96" width="0.1524" layer="91"/>
<wire x1="81.28" y1="60.96" x2="137.16" y2="60.96" width="0.1524" layer="91"/>
<junction x="81.28" y="60.96"/>
<wire x1="137.16" y1="60.96" x2="137.16" y2="63.5" width="0.1524" layer="91"/>
<pinref part="U$7" gate="G$1" pin="MOSI"/>
<wire x1="137.16" y1="63.5" x2="162.56" y2="63.5" width="0.1524" layer="91"/>
<label x="147.32" y="63.5" size="1.778" layer="95"/>
<label x="83.82" y="81.28" size="1.778" layer="95"/>
</segment>
</net>
<net name="MISO" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="12"/>
<wire x1="88.9" y1="78.74" x2="83.82" y2="78.74" width="0.1524" layer="91"/>
<wire x1="83.82" y1="78.74" x2="83.82" y2="66.04" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="MISO"/>
<wire x1="83.82" y1="66.04" x2="83.82" y2="63.5" width="0.1524" layer="91"/>
<wire x1="83.82" y1="63.5" x2="76.2" y2="63.5" width="0.1524" layer="91"/>
<pinref part="U$7" gate="G$1" pin="MISO"/>
<wire x1="162.56" y1="60.96" x2="142.24" y2="60.96" width="0.1524" layer="91"/>
<wire x1="142.24" y1="60.96" x2="142.24" y2="66.04" width="0.1524" layer="91"/>
<wire x1="83.82" y1="66.04" x2="142.24" y2="66.04" width="0.1524" layer="91"/>
<junction x="83.82" y="66.04"/>
<label x="147.32" y="60.96" size="1.778" layer="95"/>
<label x="83.82" y="78.74" size="1.778" layer="95"/>
</segment>
</net>
<net name="SCK" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="SCLK"/>
<wire x1="76.2" y1="58.42" x2="86.36" y2="58.42" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="13"/>
<wire x1="86.36" y1="58.42" x2="86.36" y2="76.2" width="0.1524" layer="91"/>
<wire x1="86.36" y1="76.2" x2="88.9" y2="76.2" width="0.1524" layer="91"/>
<pinref part="U$7" gate="G$1" pin="SCK"/>
<wire x1="162.56" y1="58.42" x2="86.36" y2="58.42" width="0.1524" layer="91"/>
<junction x="86.36" y="58.42"/>
<label x="86.36" y="76.2" size="1.778" layer="95"/>
<label x="147.32" y="58.42" size="1.778" layer="95"/>
</segment>
</net>
<net name="SDA" class="0">
<segment>
<pinref part="R3" gate="G$1" pin="2"/>
<pinref part="U$3" gate="G$1" pin="SDA"/>
<wire x1="53.34" y1="25.4" x2="66.04" y2="25.4" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="18/A4"/>
<wire x1="66.04" y1="25.4" x2="73.66" y2="25.4" width="0.1524" layer="91"/>
<wire x1="119.38" y1="86.36" x2="129.54" y2="86.36" width="0.1524" layer="91"/>
<wire x1="129.54" y1="86.36" x2="129.54" y2="35.56" width="0.1524" layer="91"/>
<wire x1="129.54" y1="35.56" x2="66.04" y2="35.56" width="0.1524" layer="91"/>
<wire x1="66.04" y1="35.56" x2="66.04" y2="25.4" width="0.1524" layer="91"/>
<junction x="66.04" y="25.4"/>
</segment>
</net>
<net name="SCL" class="0">
<segment>
<pinref part="R4" gate="G$1" pin="2"/>
<wire x1="53.34" y1="33.02" x2="60.96" y2="33.02" width="0.1524" layer="91"/>
<wire x1="60.96" y1="33.02" x2="60.96" y2="27.94" width="0.1524" layer="91"/>
<wire x1="60.96" y1="27.94" x2="73.66" y2="27.94" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="SCL"/>
<pinref part="U$1" gate="G$1" pin="19/A5"/>
<wire x1="119.38" y1="88.9" x2="134.62" y2="88.9" width="0.1524" layer="91"/>
<wire x1="134.62" y1="88.9" x2="134.62" y2="33.02" width="0.1524" layer="91"/>
<wire x1="134.62" y1="33.02" x2="60.96" y2="33.02" width="0.1524" layer="91"/>
<junction x="60.96" y="33.02"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="23/A9"/>
<wire x1="119.38" y1="99.06" x2="147.32" y2="99.06" width="0.1524" layer="91"/>
<pinref part="BUTTON_1" gate="G$1" pin="1"/>
<wire x1="147.32" y1="99.06" x2="147.32" y2="109.22" width="0.1524" layer="91"/>
</segment>
</net>
<net name="BT_IRQ" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="5"/>
<wire x1="88.9" y1="96.52" x2="73.66" y2="96.52" width="0.1524" layer="91"/>
<label x="73.66" y="96.52" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U$7" gate="G$1" pin="IRQ"/>
<wire x1="162.56" y1="68.58" x2="147.32" y2="68.58" width="0.1524" layer="91"/>
<label x="147.32" y="68.58" size="1.778" layer="95"/>
</segment>
</net>
<net name="BT_RESET" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="4"/>
<wire x1="88.9" y1="99.06" x2="73.66" y2="99.06" width="0.1524" layer="91"/>
<label x="73.66" y="99.06" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U$7" gate="G$1" pin="RST"/>
<wire x1="162.56" y1="73.66" x2="147.32" y2="73.66" width="0.1524" layer="91"/>
<label x="147.32" y="73.66" size="1.778" layer="95"/>
</segment>
</net>
<net name="BT_CS" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="6"/>
<wire x1="88.9" y1="93.98" x2="73.66" y2="93.98" width="0.1524" layer="91"/>
<label x="73.66" y="93.98" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U$7" gate="G$1" pin="CS"/>
<wire x1="162.56" y1="66.04" x2="147.32" y2="66.04" width="0.1524" layer="91"/>
<label x="147.32" y="66.04" size="1.778" layer="95"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="IC1" gate="B" pin="O"/>
<pinref part="LED_CLK" gate="G$1" pin="1"/>
<wire x1="25.4" y1="86.36" x2="17.78" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="LED_DATA" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="7"/>
<wire x1="88.9" y1="91.44" x2="66.04" y2="91.44" width="0.1524" layer="91"/>
<wire x1="66.04" y1="91.44" x2="66.04" y2="114.3" width="0.1524" layer="91"/>
<pinref part="IC1" gate="A" pin="I"/>
<wire x1="66.04" y1="114.3" x2="60.96" y2="114.3" width="0.1524" layer="91"/>
<label x="73.66" y="91.44" size="1.778" layer="95"/>
</segment>
</net>
<net name="LED_CLK" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="14/A0"/>
<wire x1="119.38" y1="76.2" x2="124.46" y2="76.2" width="0.1524" layer="91"/>
<wire x1="124.46" y1="76.2" x2="124.46" y2="68.58" width="0.1524" layer="91"/>
<wire x1="124.46" y1="68.58" x2="60.96" y2="68.58" width="0.1524" layer="91"/>
<label x="111.76" y="68.58" size="1.778" layer="95"/>
<pinref part="IC1" gate="B" pin="I"/>
<wire x1="60.96" y1="86.36" x2="45.72" y2="86.36" width="0.1524" layer="91"/>
<label x="48.26" y="86.36" size="1.778" layer="95"/>
<wire x1="60.96" y1="68.58" x2="60.96" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
<errors>
<approved hash="204,1,119.38,104.14,U$1,AGND,,,,"/>
<approved hash="204,1,119.38,109.22,U$1,3.3V,,,,"/>
<approved hash="104,1,43.18,53.34,U$2,VIN_5V,VIN,,,"/>
<approved hash="114,1,50.4611,109.897,IC1,C,OE,,,"/>
<approved hash="114,1,50.4611,109.897,IC1,C,I,,,"/>
<approved hash="114,1,50.4611,109.897,IC1,D,OE,,,"/>
<approved hash="114,1,50.4611,109.897,IC1,D,I,,,"/>
<approved hash="104,1,15.24,25.4,IC1P,VCC,VIN,,,"/>
</errors>
</schematic>
</drawing>
<compatibility>
<note version="6.3" minversion="6.2.2" severity="warning">
Since Version 6.2.2 text objects can contain more than one line,
which will not be processed correctly with this version.
</note>
</compatibility>
</eagle>
