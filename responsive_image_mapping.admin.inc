<?php
/**
 * @file
 * 
 */

/**
 * Respinsive Image Mapping configuration page.
 */
function responsive_image_mapping_settings() {
  /* Let's Do Stuff Here! */

  $form['#submit'][] = 'responsive_image_mapping_admin_settings_submit';

  return system_settings_form($form);
}

