# ESP32_Cam_Pan_Tilt

ESP-32 cam project with pan/tilt servers and a 3D printed case based on 
Rui Santos's tutorial on this subject:
https://randomnerdtutorials.com/esp32-cam-pan-and-tilt-2-axis/
and Andreas Spiess's tutorial on creating 3D printed paramatized cases for our projects https://www.youtube.com/watch?v=lBK0UBjVrYM

This project is heavily based on the  above link. I have added a couple features and accessories

1: Added the ability to "sweep" the servos left, right, up or down and "home".

2: Added the ability to rotate the image left or right (different camera lenses do portrait or landscape by default). This function was inspired by this link
https://diyprojects.io/5-tips-esp32-cam-fixed-ip-address-ap-mode-image-rotation-90-automatic-recovery-wifi-connection-html-code-storage/#.YW96YVXMJhE

3: Included a fritzing file for the circuit board I desiged and built. NOTE, this is a thru-hole design using an off the shelf breadboard. Link for the breadboard included in the BOM. This could be made smaller by using a printed circuit board, but I have no need to do so.

4: Included .stl files for both the regular lens included with the esp32-cam and the wide angle lens bought seperatly. Both were inspired by Andreas Spiess's great tutorial on creating peramertized cases for electronics - https://www.youtube.com/watch?v=lBK0UBjVrYM

Some Pic's:

![20220506_222708](https://user-images.githubusercontent.com/31168214/167240507-1534a5f7-2af8-4ee1-83a0-ae81b6e803d7.jpg)

![20220506_222733](https://user-images.githubusercontent.com/31168214/167240512-dd1db737-fcff-44f2-a638-5a24873fa72e.jpg)

![20220506_223007](https://user-images.githubusercontent.com/31168214/167240514-2c0a2f19-33b8-47d3-b4a8-234b3589b61f.jpg)

BOM:
1: 3d printer black filament https://www.amazon.com/gp/product/B085NVCSWW/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1

2:ESP32-cam wide angle lens
https://www.amazon.com/gp/product/B0894KKXHX/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

3:ESP32-cam (2-pack) 
https://www.amazon.com/gp/product/B07S5PVZKV/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

Solderable PC Board
https://www.amazon.com/gp/product/B0040Z6OK6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

Mini Pan/tilt Mount with Servos
https://www.amazon.com/gp/product/B0775R6JFF/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
btw, I bought this off of Amazon but you can get it cheaper from other overseas vendors.

Misc electronic parts - headers, jumpers, capacitors etc



