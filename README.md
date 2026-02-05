<h1 align=center>42_LED_MATRIX</h1>
<p align=center>
  <img src="photos/1770282287803.jpg" alt="led-matrix-photo" style="width:50%; height:auto;">
</p>

An educational electronics project built after attending the **42 Embedded Pool**, showcased during the opening night of the new 42 Paris building, in partnership with **42’s Electronics Lab (Labelec)**.

## 🧠 Project Overview

At 42, during the common core, we mostly study computer science and spend a lot of time in front of a screen (which I don’t dislike!). This project was a great opportunity to step away from pure software and **build a tangible, real-time system**, dealing with **physical constraints and hardware limitations**.

The original idea was to create an LED matrix displaying geometric animations that would morph based on the viewer’s distance. Due to time constraints and imperfect hardware choices (ultrasonic sensors being imprecise and tricky to work with), the sensors ultimately became triggers for predefined animations instead.

It was made in about two months.

## ⚙️ Hardware & Architecture

The project runs on an **ESP32**, using the **RMT module** to send color data to **WS2811 addressable LEDs** asynchronously. The LEDs are chained in series: each LED consumes its data and forwards the rest to the next one.

Because ultrasonic distance measurements are blocking by nature, they initially interfered with animation playback. To handle this, **multithreading** was originally implemented to **meet real-time constraints** and keep animations smooth.

The project uses the ESP-IDF libray. Other then that, it is written entirely in C.

## Components

- ESP32

- 300 WS2811 addressable LEDs

- 2× HC-SR04 ultrasonic distance sensors

- Sound sensor, in a 3D printed casing

## 🧪 Lessons Learned

- Designing for real-time constraints is very different from pure software

- Hardware choices matter (a lot!)

- Multithreading is often unavoidable in interactive systems

- Prototyping in the physical world is messy—but very rewarding

## 🖼️ Gallery
<p style="display: flex">
  <img src="photos/1770282287803.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770282287834.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770286963458.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770286963471.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770286963484.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770286963497.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770286963509.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
  <img src="photos/1770287208114.jpg" alt="led-matrix-photo" style="width:20%; height:auto;">
</p>
<p align=center>
  <a href="https://youtu.be/6xFcAzCB4g4">
    <img src="https://img.youtube.com/vi/6xFcAzCB4g4/0.jpg"/>
  </a>
</p>

##
<!-- <div style="display: flex">
  <img src="https://cdn.brandfetch.io/idyW54xW9w/w/789/h/789/theme/dark/icon.jpeg?c=1bxid64Mup7aczewSAYMX&t=1768254948784" style="width: 10%"/>
  <p style="width: 50%">
    This project would not have been possible without the support and guidance of 42’s Labelec, as well as the 42 staff who funded the project.
    Many thanks to them !
  </p>
</div> -->

<table>
  <tr>
    <td width="120">
      <img src="https://cdn.brandfetch.io/idyW54xW9w/w/789/h/789/theme/dark/icon.jpeg?c=1bxid64Mup7aczewSAYMX&t=1768254948784" width="80"/>
    </td>
    <td align="center">
      This project would not have been possible without the support and guidance of
      <strong>42’s Labelec</strong>, as well as the <strong>42 staff</strong> who funded the project.
      <br/>
      Many thanks to them!
    </td>
  </tr>
</table>
