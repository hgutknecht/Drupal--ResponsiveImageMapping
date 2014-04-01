(function($) {
  /**
   * Determine which images are responsive and mapped.
   *
   */
  Drupal.behaviors.responsiveImageHandler = {
    attach: function (context, settings) {
      // on window load fire off swap
      Drupal.behaviors.responsiveImageHandler.imgSwap();
      // Potentially add window resize handling with timeout
    },
    imgSwap: function() {
      var currentBreakpointMapping = ''
        , currentBreakpoint = 0
        , breakpointCounter = 0
        , images = ''
        , currentImage = ''
        , imageLength = 0
        , newImg = ''
        , newImgSource = ''
        , imageCount = 0
        , attrCount = 0
        , placeholder = 0;
      // Check if we are using blank gif placeholders.
      placeholder = Drupal.settings.responsive_image_mapping.tinyGIF;
      // Calculate current breakpoint.
      currentBreakpoint = Drupal.behaviors.rimHelpers.calcBreakpoint();
      if (currentBreakpoint >= 0) {
        currentBreakpointMapping = 'data-responsive_image_mapping_' + currentBreakpoint;
      }
      else {
        // If we do not match a break, fallback to default.
        currentBreakpointMapping = 'data-responsive_image_mapping_preset';
      }
      // Gather all responsive images.
      images = document.querySelectorAll('img.responsive_image_mapping');
      imageLength = images.length;
      for (imageCount = 0; imageCount < imageLength; imageCount++) {
        // If we are on the smallest breakpoint (data-responsive_image_mapping_0)
        // and are serving the smallest image no need to imgSwap.
        if (currentBreakpointMapping === 'data-responsive_image_mapping_0' && placeholder === 1) {
          // Left in case we need to do some processing here...
        }
        else {
          // Create the new image and swap in DOM.
          currentImage = images[imageCount];
          newImgSource = currentImage.getAttribute(currentBreakpointMapping);
          breakpointCounter = currentBreakpoint;
          while (newImgSource === 'none') {
            newImgSource = currentImage.getAttribute('data-responsive_image_mapping_' + (breakpointCounter++));
          }
          if (newImgSource === null || newImgSource === '') {
            newImgSource = currentImage.getAttribute('data-responsive_image_mapping_preset');
          }
          newImg = new Image();
          newImg.onload = function() {
            newImg.onload = null;
            var imgAttributes = currentImage.attributes;
            var imgAttributeLength = imgAttributes.length;
            for (attrCount = 0; attrCount < imgAttributeLength; attrCount++) {
              if (imgAttributes[attrCount].name != 'height' && imgAttributes[attrCount].name != 'width' && imgAttributes[attrCount].name != 'src' && imgAttributes[attrCount].name != 'data-borealis-style' && imgAttributes[attrCount].name != 'data-locked') {
                newImg.setAttribute(imgAttributes[attrCount].name, imgAttributes[attrCount].value);
              }
            }
            currentImage.parentNode.replaceChild(newImg, currentImage);
          }
          newImg.src = newImgSource;
        }
      }
    }
  };

  /**
   * Helper functions for responsive image mapping.
   *
   */
  Drupal.behaviors.rimHelpers = {
     /**
     * Calculate viewport height and width.
     * @return array
     */
    getViewport: function() {
      var e = window
        , a = 'inner';
      if (!('innerWidth' in window)) {
        a = 'client';
        e = document.documentElement || document.body;
      }
      return {
        width : e[ a+'Width' ],
        height : e[ a+'Height' ]
      }
    },
    /**
     * Calculate active breakpoint.
     * -1 represents no break identified, use default.
     * @return int - index of active breakpoint.
     */
    calcBreakpoint: function() {
      var breakpoints = ''
        , viewportSize = ''
        , previousBreak = 0
        , breakpointIndex = 0
        , activeBreak = -1;
      breakpoints = Drupal.settings.responsive_image_mapping.mappings;
      breakpoints.forEach(function(breakpoint) {
        viewportSize = Drupal.behaviors.rimHelpers.getViewport();
        if ((viewportSize.width > previousBreak) && (viewportSize.width < breakpoint)) {
          activeBreak = breakpointIndex;
        }
        previousBreak = breakpoint;
        breakpointIndex++;
      });
      return activeBreak;
    }
  };

})(jQuery);
