<?php
 // Callback function for interpreting 0b00 syntax
 function callbackBinary($match) {
  $s = 0;
  for($i = 0; $i < strlen($match[1]); $i++) {
    $s += intval($match[1][strlen($match[1])-$i-1]) * pow(2, $i);
  }
  return $s;
 }
 
 // Interpret font.h
 $file = file_get_contents('../marquee2/font.h');
 $temp = substr($file, strpos($file, '{') + 1, strrpos($file, '}') - strpos($file, '{') - 1);
 $temp = preg_replace_callback('/0b([01]{1,8})/', callbackBinary, $temp);
 eval('$font = array(' . $temp . ');');
 
 // Create image
 header('Content-type: image/png');
 $im = imagecreatetruecolor(count($font), 8);
 $color = imagecolorallocate($im, 255, 255, 0);
 for($x = 0; $x < count($font); $x++) {
  for($y = 0; $y < 8; $y++) {
   if(($font[$x] | pow(2, 7-$y)) == $font[$x]) imagesetpixel($im, $x, $y, $color);
  }
 }
 imagepng($im);
?>
