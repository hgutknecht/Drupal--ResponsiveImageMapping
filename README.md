Responsive Image Mapping (Drupal)
-------------------
[Project page on d.o](https://www.drupal.org/project/responsive_image_mapping)

Experimental Drupal module to replace image source based on image preset mappings.

Greatly inspired by the [Borealis Responsive Image project](https://drupal.org/project/borealis), this module uses the same image replacement theory, Displaying a small image initially and, as the images get larger, swapping them out for a larger image.

The difference is that Borealis creates 7 new image styles for each image style you make responsive and maps them based on the module's calculated breakpoints.

With this module we map one already existing image preset to another existing image preset. You are responsible for creating and mapping to the presets. This would allow you for instance, to map a landscape image on a XXL display to a square cropping for medium viewports and a rectangular cropping for mobile. NOTE: This module does not currently swap images on window resize.

###To setup.

  1. Define your breakpoints
  2. Chose whether or not to display a blank placeholder or smallest image by default.
  3. Chose the image preset you would like to be responsive and chose it's breakpoints.
  4. Rinse and repeat for all presets you would like to be made responsive.


###Future enhancements

  1. We will not be integrating with the Breakpoints module
  2. Utilize element queries or parent width instead of window width
  3. Lazy loading on scroll
  4. window resize replacement
  5. Element queries

I will be iterating on this module and adding additional functionality. If you have a feature request just leave a note in the issue queue.
