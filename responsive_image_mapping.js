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
        , images = ''
        , currentImage = ''
        , imageLength = 0
        , newImg = ''
        , newImgSource = ''
        , imageCount = 0
        , attrCount = 0
        , placeholder = 0
        , elementQueryWidth = '';
      // Check if we are using blank gif placeholders.
      placeholder = Drupal.settings.responsive_image_mapping.tiny_gif;
      // Gather all responsive images.
      images = document.querySelectorAll('img.responsive_image_mapping');
      imageLength = images.length;
      for (imageCount = 0; imageCount < imageLength; imageCount++) {
        currentImage = images[imageCount];
        // Query the element container width.
        // We are making the assumption of an <a> wrapper so we get the parent of that.
        elementQueryWidth = $(currentImage).parent().parent().width();
        // Set current breakpoint based on element query.
        currentBreakpoint = Drupal.behaviors.rimHelpers.calcBreakpoint(elementQueryWidth);
        if (currentBreakpoint >= 0) {
          currentBreakpointMapping = 'data-responsive_image_mapping_' + currentBreakpoint;
        }
        else {
          // If we do not match a break, fallback to default.
          currentBreakpointMapping = 'data-responsive_image_mapping_preset';
        }
        // If we are on the smallest breakpoint (data-responsive_image_mapping_0)
        // and are serving the smallest image no need to imgSwap.
        if (currentBreakpointMapping === 'data-responsive_image_mapping_0' && placeholder === 1) {
          // Left in case we need to do some processing here...
        }
        else {
          // Create the new image and swap in DOM.
          newImgSource = currentImage.getAttribute(currentBreakpointMapping);
          // Fallback to preset if not defined for some reason.
          if (newImgSource === null || newImgSource === '') {
            newImgSource = currentImage.getAttribute('data-responsive_image_mapping_preset');
          }
          newImg = new Image();
          newImg.onload = function() {
            newImg.onload = null;
            var imgAttributes = currentImage.attributes;
            var imgAttributeLength = imgAttributes.length;
            for (attrCount = 0; attrCount < imgAttributeLength; attrCount++) {
              if (imgAttributes[attrCount].name != 'height' && imgAttributes[attrCount].name != 'width' && imgAttributes[attrCount].name != 'src' && imgAttributes[attrCount].name != 'data-locked' && (imgAttributes[attrCount].name).indexOf('data-responsive_image_mapping_')) {
                newImg.setAttribute(imgAttributes[attrCount].name, imgAttributes[attrCount].value);
              }
            }
          }
          currentImage.parentNode.replaceChild(newImg, currentImage);
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
     * Deprected, To be removed.
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
     * @param elementQueryWidth width in pixels of the images parent().parent()
     * @return int - index of active breakpoint.
     */
    calcBreakpoint: function(elementQueryWidth) {
      var breakpoints = ''
        , viewportSize = ''
        , previousBreak = 0
        , breakpointIndex = 0
        , activeBreak = -1;
      breakpoints = Drupal.settings.responsive_image_mapping.mappings;
      breakpoints.forEach(function(breakpoint) {
        breakpoint = parseInt(breakpoint);
        if ((elementQueryWidth > previousBreak) && (elementQueryWidth < breakpoint)) {
          activeBreak = breakpointIndex;
        }
        previousBreak = breakpoint;
        breakpointIndex++;
      });
      return activeBreak;
    }
  };

})(jQuery);
