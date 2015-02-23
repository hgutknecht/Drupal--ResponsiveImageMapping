README.txt
==========
An experimental module to read in the "image styles" presets and dynamically map them to another preset, or to not show the image, based on display size.

When the setting is to not show the image for a specific device it remove the image from the DOM as well as the http request for the image. Hook_init?

This will utilize either adapt.js, straight CSS3vmedia queries or a PHP solution to determine the users device.

Via the settings, the UI will allow the user to map an image preset to another or to display no image at all for that device.

Roadmap...
1. General module framework
  -- admin settings DONE
  -- perms DONE
2. Hook into image, and image styles form
  -- http://api.drupal.org/api/drupal/modules--image--image.api.php/7
  -- ~ http://api.drupal.org/api/drupal/modules--image--image.api.php/function/hook_image_default_styles/7
3. Allow users to select which image styles can be responsive
  -- Need UI design
4. Form alter to switch to new style or to DISPLAY NONE
5. Figure out PHP solution to determine display width
  -- on less than 440px width screens, cycle through image styles and swap out for responsive images or if set tro DISPLAY none, remove image request all together


Dependencies
==========


AUTHOR/MAINTAINER
======================
Hans Gutknecht
hansyg on d.o


