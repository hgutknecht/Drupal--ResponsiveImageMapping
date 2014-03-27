<?php

/**
 * Responsive Image mapping admin.
 *
 * @return Form for settings and adding mappings.
 */
function responsive_image_mapping_admin_form($form, &$form_state) {
  // Fetch image styles. Add names to array for form.
  $styles = image_styles();
  $style_names = array(
    'none' => 'none',
  );
  foreach ($styles as $style_name => $style) {
    $style_names[$style_name] = $style_name;
  }
  $form['rim_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Responsive image mapping settings.'),
  );
  $form['rim_settings']['rim_small_breakpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Small Breakpoint'),
    '#description' => t('Breakpoint for small viewports in pixels.'),
    '#default_value' => variable_get('rim_small_breakpoint', ''),
  );
  $form['rim_settings']['rim_medium_breakpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Medium Breakpoint'),
    '#description' => t('Breakpoint for medium viewports in pixels.'),
    '#default_value' => variable_get('rim_medium_breakpoint', ''),
  );
  $form['rim_settings']['rim_image_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Image default.'),
    '#description' => t('Show small image by default for images with mappings. If not checked a blank gif will be used as a placeholder.'),
    '#default_value' => variable_get('rim_image_default', FALSE),
  );
  // Add mappings form.
  $form['rim_add_mapping'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Image preset mappings.'),
  );
  $form['rim_add_mapping']['rim_preset_key'] = array(
    '#type' => 'select',
    '#title' => t('Preset Key'),
    '#options' => $style_names,
    '#description' => t('Choose the image preset to map. If no preset is chosen here, no mapping will be created.'),
    '#default_value' => variable_get('rim_preset_key', array()),
  );
  $form['rim_add_mapping']['rim_mapping_1'] = array(
    '#type' => 'select',
    '#title' => t('Small Breakpoint Mapping'),
    '#options' => $style_names,
    '#description' => t('Choose the default preset to be used for small screens. Note this preset will be used for default if the option is chosen above.'),
    '#default_value' => variable_get('rim_mapping_1', array()),
  );
  $form['rim_add_mapping']['rim_mapping_2'] = array(
    '#type' => 'select',
    '#title' => t('Medium Breakpoint'),
    '#options' => $style_names,
    '#description' => t('Choose the default preset to be used for medium screens. Viewports inbetween the breakpoints assigned above.'),
    '#default_value' => variable_get('rim_mapping_2', array()),
  );
  // Existing mappings.
  $current_mappings = responsive_image_mapping_get_mappings();
  if ($current_mappings) {
    $form['rim_mappings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#title' => t('Current mappings.'),
    );

    foreach ($current_mappings as $key => $value) {
      $form['rim_mappings']['mapping_' . $key] = array(
        '#prefix' => '<div>',
        '#markup' => 'Responsive preset: ' . $current_mappings[$key]['preset_key'] . ' Mapping 1: ' . $current_mappings[$key]['mappings'][0] . ' -> Mapping 2: ' . $current_mappings[$key]['mappings'][1],
        '#suffix' => '</div>',
      );
    }
  }

  $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Save settings and mapping.'),
  );
  return $form;
}

/**
 * Submit handler for responsive_image_mapping_admin_form.
 *
 */
function responsive_image_mapping_admin_form_submit($form, &$form_state) {
  // Save the variables.
  variable_set('rim_small_breakpoint', $form_state['values']['rim_small_breakpoint']);
  variable_set('rim_medium_breakpoint', $form_state['values']['rim_medium_breakpoint']);
  variable_set('rim_image_default', $form_state['values']['rim_image_default']);
  // Save the mappings to database.
  $mappings = array(
    0 => $form_state['values']['rim_mapping_1'],
    1 => $form_state['values']['rim_mapping_2'],
  );
  db_insert('responsive_img_mapping') // Table name no longer needs {}
    ->fields(array(
      'preset_key' => $form_state['values']['rim_preset_key'],
      'mappings' => serialize($mappings),
    ))
    ->execute();
  drupal_set_message(t('Mapping saved.'));
}

/**
 * responsive_image_mapping_edit_form
 * Form callback for editing mappings.
 *
 * @return Form for editing mapping.
 */
// function responsive_image_mapping_edit_form() {
//   $form = array();
//   $id = arg(5);
//   $tag = custom_meta_get_tags($id);


//   $form['submit'] = array(
//    '#type' => 'submit',
//    '#value' => t('Save mapping'),
//   );

//   $form['cancel'] = array(
//    '#markup' => '<a href="/admin/config/search/custom_meta">' . t('Cancel') . '</a>',
//   );

//   return $form;
// }

/**
 * Fetch all or specified mappings from db.
 * @return (array) returns mappings.
 *
 */
function responsive_image_mapping_get_mappings($id = '') {
  if ($id) {
    $result = db_query("SELECT * FROM {responsive_img_mapping} WHERE id=:id", array(':id' => $id));
    return $result->fetchAssoc();
  }
  else {
    $mappings = db_query('SELECT * FROM {responsive_img_mapping}');
    while ($row = $mappings->fetchAssoc()) {
      $return[] = $row;
    }
    foreach ($return as $key => $value) {
      $return[$key]['mappings'] = unserialize($return[$key]['mappings']);
    }
    return $return;
  }
}
