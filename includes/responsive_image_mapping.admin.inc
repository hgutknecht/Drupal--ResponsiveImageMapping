<?php
/**
 * @file
 * Admin forms and functinality for responsive image mappings.
 */

/**
 * Responsive Image mapping admin.
 *
 * @return Form for settings and adding mappings.
 */
function responsive_image_mapping_admin_form($form, &$form_state) {
  $style_names = responsive_image_mapping_get_image_style_names();
  $form['settings'] = array(
    '#type' => 'vertical_tabs',
  );
  $form['settings']['rim_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Breakpoint settings.'),
    '#description' => t('To remove a breakpoint leave the field blank and save.')
  );
  $current_bps = variable_get('rim_breakpoints', 0);
  if ($form_state['rebuild'] === TRUE) {
    $bp_count = $form_state['breakpoints'];
  }
  elseif (isset($current_bps)) {
    $bp_count = count($current_bps);
  }
  else {
    $bp_count = 1;
  }
  $bp_counter = 0;
  for ($bp_counter; $bp_counter < $bp_count; $bp_counter++) {
    $form['settings']['rim_settings']['breakpoint_' . $bp_counter] = array(
      '#type' => 'textfield',
      '#title' => check_plain(t('Breakpoint') . ' ' . ($bp_counter + 1)),
      '#default_value' => isset($current_bps[$bp_counter]) ? $current_bps[$bp_counter] : ''
    );
  }
  $form['settings']['rim_mappings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Mapping settings.'),
  );
  $form['settings']['rim_settings']['#bp_count'] = $bp_count;
  $form['settings']['rim_settings']['add_bp'] = array(
    '#type' => 'submit',
    '#name' => 'add_bp',
    '#value' => t('Add another breakpoint')
  );
  $form['settings']['rim_settings']['rim_image_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Image default.'),
    '#description' => t('Show small image by default for images with mappings. If not checked a blank gif will be used as a placeholder.'),
    '#default_value' => variable_get('rim_image_default', FALSE),
  );
  // Existing mappings.
  $current_mappings = responsive_image_mapping_get_mappings();
  if ($current_mappings) {
    $mappings_markup['header'] = array('Preset Key');
    foreach ($current_bps as $key => $value) {
      $mappings_markup['header'][] = $value;
    }
    $mappings_markup['header'][] = 'Actions';
    foreach ($current_mappings as $key => $value) {
      $mappings_markup['rows'][$key] = array($current_mappings[$key]['preset_key']);
      $mappings_counter = 0;
      for ($mappings_counter; $mappings_counter < $bp_count; $mappings_counter++) {
        $style_mapping = isset($current_mappings[$key]['mappings'][$mappings_counter]) ? $current_mappings[$key]['mappings'][$mappings_counter] : '';
        if ($style_mapping) {
          $mappings_markup['rows'][$key][] = $style_mapping;
        }
        else {
          $mappings_markup['rows'][$key][] = 'none';
        }
      }
      $mappings_markup['rows'][$key][] = '<div class="actions"><a href="/admin/config/media/image-styles/responsive-mappings/edit/' . $current_mappings[$key]['id'] . '">edit</a> | <a href="/admin/config/media/image-styles/responsive-mappings/remove/' . $current_mappings[$key]['id'] . '">remove</a></div>';
    }
    $form['settings']['rim_mappings']['existing'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#title' => t('Existing mappings.'),
    );

    $form['settings']['rim_mappings']['existing']['table'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="rim-mappings-wrapper">',
      '#markup' => theme('table', $mappings_markup),
      '#suffix' => '</div>',
    );
  }
  $form['settings']['rim_mappings']['add_mapping'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Add Mapping'),
    '#description' => t('Mapping 1 is the default preset for small screens. Note that it will be used as the default if the "Image Default" option is checked.'),
  );
  $form['settings']['rim_mappings']['add_mapping']['rim_preset_key'] = array(
    '#type' => 'select',
    '#title' => t('Preset Key'),
    '#options' => $style_names,
    '#description' => t('Choose the image preset to map. If no preset is chosen here, no mapping will be created.'),
    '#default_value' => variable_get('rim_preset_key', array()),
  );
  $bp_counter = 0;
  for ($bp_counter; $bp_counter < $bp_count; $bp_counter++) {
    $form['settings']['rim_mappings']['add_mapping']['rim_mapping_' . $bp_counter] = array(
      '#type' => 'select',
      '#title' => check_plain(t('Mapping') . ' ' . ($bp_counter + 1)),
      '#options' => $style_names
    );
  }
  $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Save settings and mapping.'),
  );
  return $form;
}

/**
 * Form validation for responsive_image_mapping_admin_form.
 *
 */
function responsive_image_mapping_admin_form_validate($form, &$form_state) {
  if ($form_state['triggering_element']['#name'] !== 'add_bp') {
    $preset_key = $form_state['values']['rim_preset_key'];
    // Check to make sure preset mapping isn't already set.
    $current_mappings = responsive_image_mapping_get_mappings();
    if ($current_mappings) {
      foreach ($current_mappings as $key => $value) {
        if ($current_mappings[$key]['preset_key'] == $preset_key) {
          form_set_error('rim_preset_key', t('A mapping already exists for that preset, please edit or delete the existing one.'));
        }
      }
    }
    $form_keys = array_keys($form_state['values']);
    $new_mappings = 0;
    foreach ($form_keys as $value) {
      if (strpos($value, 'rim_mapping_') !== false) {
        $new_mappings++;
      }
    }
    // Check for empty mappings, we want to ensure there is at least one mapping.
    if ($new_mappings > 1 && $preset_key !== 'none') {
      $mapping_set = FALSE;
      // Check all mapping values, check that all are not set to 'none'.
      for ($i = 0; $i < $new_mappings; $i++) {
        $index_value = 'rim_mapping_' . $i;
        if ($form_state['values'][$index_value] !== 'none') {
          $mapping_set = TRUE;
        }
        echo $i;
      }
      if (!$mapping_set) {
        form_set_error('mapping_1', t('You must select at least one mapping.'));
      }
    }
    elseif ($preset_key !== 'none' && $form_state['values']['rim_mapping_0'] == 'none') {
      // Single mapping, ensure the single mapping is not 'none'.
      form_set_error('mapping_1', t('You must select at least one mapping.'));
    }
  }
}

/**
 * Submit handler for responsive_image_mapping_admin_form.
 *
 */
function responsive_image_mapping_admin_form_submit($form, &$form_state) {
  if ($form_state['triggering_element']['#name'] === 'add_bp') {
    $form_state['breakpoints'] = $form['settings']['rim_settings']['#bp_count'] + 1;
    $form_state['rebuild'] = TRUE;
    drupal_set_message(t('You are adding a breakpoint, you may have to edit your existing mappings.'), 'warning');
  }
  else {
    $breakpoints = array();
    $mappings = array();
    $current_mappings = responsive_image_mapping_get_mappings();
    foreach ($form_state['values'] as $form_key => $form_value) {
      if (preg_match('/breakpoint_+/', $form_key) && !empty($form_value)) {
        $breakpoints[] = $form_value;
      }
      elseif (preg_match('/rim_mapping_+/', $form_key) && !empty($form_value)) {
        $mappings[] = $form_value;
      }
      // Save mappings if breakpoint is deleted and a current mapping references that break.
      elseif (empty($form_value) && strpos($form_key, 'breakpoint') !== FALSE) {
        $missing_key = str_replace('breakpoint_', '', $form_key);
        $missing_key = $missing_key - 1;
        foreach ($current_mappings as $key => $mapping) {
          $curr_maps_changed = FALSE;
          if (isset($current_mappings[$key]['mappings'][$missing_key])) {
            unset($current_mappings[$key]['mappings'][$missing_key]);
            $current_mappings[$key]['mappings'] = array_values($current_mappings[$key]['mappings']);
            $curr_maps_changed = TRUE;
          }
          if ($curr_maps_changed) {
            db_update('responsive_img_mapping')
              ->fields(array(
                'preset_key' => $current_mappings[$key]['preset_key'],
                'mappings' => serialize($current_mappings[$key]['mappings'])
              ))
              -> condition('id', $current_mappings[$key]['id'])
              ->execute();
            $message = 'You removed breakpoint ' . $form_key . ', ' . $current_mappings[$key]['preset_key'] . ' mapping has been updated accordingly.';
            drupal_set_message(check_plain(t($message)), 'warning');
          }
        }
      }
    }
    variable_set('rim_breakpoints', $breakpoints);
    variable_set('rim_image_default', $form_state['values']['rim_image_default']);
    // Save the mappings to database.
    if ($form_state['values']['rim_preset_key'] !== 'none') {
      db_insert('responsive_img_mapping') // Table name no longer needs {}
        ->fields(array(
          'preset_key' => $form_state['values']['rim_preset_key'],
          'mappings' => serialize($mappings),
        ))
        ->execute();
      drupal_set_message(t('Setting saved. Mapping created.'));
    }
    else {
      drupal_set_message(t('Setting saved. No mapping created, no preset key selected.'));
    }
  }
}

/**
 * responsive_image_mapping_edit_form
 * Form callback for editing mappings.
 *
 * @return Form for editing mapping.
 */
function responsive_image_mapping_edit_form() {
  $form = array();
  $id = arg(6);
  $tag = responsive_image_mapping_get_mappings($id);
  $style_names = responsive_image_mapping_get_image_style_names();
  $form['settings']['rim_add_mapping']['rim_preset_key'] = array(
    '#type' => 'select',
    '#title' => t('Preset Key'),
    '#options' => $style_names,
    '#description' => t('Choose the image preset to map. If no preset is chosen here, no mapping will be created.'),
    '#default_value' => variable_get('rim_preset_key', $tag['preset_key']),
  );
  $current_bps = variable_get('rim_breakpoints', 1);
  if (isset($current_bps)) {
    $bp_count = count($current_bps);
  }
  else {
    $bp_count = 1;
  }
  $bp_counter = 0;
  for ($bp_counter; $bp_counter < $bp_count; $bp_counter++) {
    $form['settings']['rim_add_mapping']['rim_mapping_' . $bp_counter] = array(
      '#type' => 'select',
      '#title' => check_plain(t('Mapping') . ' ' . ($bp_counter + 1)),
      '#options' => $style_names,
      '#default_value' => isset($tag['mappings'][$bp_counter]) ? $tag['mappings'][$bp_counter] : 'none',
    );
  }
  $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Save mapping'),
  );
  $form['cancel'] = array(
   '#markup' => '<a href="/admin/config/media/image-styles/responsive-mappings">' . t('Cancel') . '</a>',
  );
  return $form;
}

/**
 * Submit handler for responsive_image_mapping_edit_form.
 *
 */
function responsive_image_mapping_edit_form_submit($form, &$form_state) {
  // Save the edited mapping to database.
  if ($form_state['values']['rim_preset_key'] !== 'none') {
    $mappings = array();
    foreach ($form_state['values'] as $form_key => $form_value) {
      if (preg_match('/rim_mapping_[0-9]+/', $form_key) && !empty($form_value)) {
        $mappings[] = $form_value;
      }
    }
    db_update('responsive_img_mapping')
      ->fields(array(
        'preset_key' => $form_state['values']['rim_preset_key'],
        'mappings' => serialize($mappings)
      ))
      -> condition('id', arg(6))
      ->execute();
    drupal_set_message(t('Mapping updated.'));
    drupal_goto('admin/config/media/image-styles/responsive-mappings');
  }
  else {
    drupal_set_message(t('Mapping not updated, no preset key selected.'));
    drupal_goto('admin/config/media/image-styles/responsive-mappings');
  }
}

/**
 * Fetch image styles. Add names to array for form.
 * @return (array) returns image style names.
 *
 */
function responsive_image_mapping_get_image_style_names() {
  $styles = image_styles();
  $style_names = array(
    'none' => 'none',
  );
  foreach ($styles as $style_name => $style) {
    $style_names[$style_name] = $style_name;
  }
  return $style_names;
}

/**
 * Fetch all or specified mappings from db.
 * @return (array) returns mappings.
 *
 */
function responsive_image_mapping_get_mappings($id = '') {
  $return = '';
  if ($id) {
    $result = db_query("SELECT * FROM {responsive_img_mapping} WHERE id=:id", array(':id' => $id));
    $mapping = $result->fetchAssoc();
    $mapping['mappings'] = unserialize($mapping['mappings']);
    return $mapping;
  }
  else {
    $mappings = db_query('SELECT * FROM {responsive_img_mapping}');
    while ($row = $mappings->fetchAssoc()) {
      $return[] = $row;
    }
    if ($return) {
      foreach ($return as $key => $value) {
        $return[$key]['mappings'] = unserialize($return[$key]['mappings']);
      }
      return $return;
    }
    else {
      return $return;
    }
  }
}

/**
 * Remove a mapping from the database.
 *
 */
function responsive_image_mapping_remove_mapping() {
  $id = arg(6);
  db_delete('responsive_img_mapping')
    ->condition('id', $id)
    ->execute();
  drupal_set_message(t('Mapping removed.'));
  drupal_goto('admin/config/media/image-styles/responsive-mappings');
}
