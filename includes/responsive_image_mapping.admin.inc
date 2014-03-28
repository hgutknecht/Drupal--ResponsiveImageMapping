<?php

/**
 * Responsive Image mapping admin.
 *
 * @return Form for settings and adding mappings.
 */
function responsive_image_mapping_admin_form($form, &$form_state) {
  $style_names = responsive_image_mapping_get_image_style_names();
  $form['rim_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Responsive image mapping settings.'),
  );
  // In the future we can do an ajax callback for the client to add as many breaks as they would like.
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
        '#markup' => '<strong>Responsive preset:</strong> ' . $current_mappings[$key]['preset_key'] . ' -> Mapping 1: ' . $current_mappings[$key]['mappings'][0] . ' -> Mapping 2: ' . $current_mappings[$key]['mappings'][1] . '<div class="actions"><a href="/admin/config/media/image-styles/responsive-mappings/edit/' . $current_mappings[$key]['id'] . '">edit</a> | <a href="/admin/config/media/image-styles/responsive-mappings/remove/' . $current_mappings[$key]['id'] . '">remove</a></div>',
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
 * Form validation for responsive_image_mapping_admin_form.
 *
 */
function responsive_image_mapping_admin_form_validate($form, &$form_state) {
  $current_mappings = responsive_image_mapping_get_mappings();
  $preset_key = $form_state['values']['rim_preset_key'];
  $mapping_1 = $form_state['values']['rim_mapping_1'];
  $mapping_2 = $form_state['values']['rim_mapping_1'];
  foreach ($current_mappings as $key => $value) {
    if ($current_mappings[$key]['preset_key'] == $preset_key) {
      form_set_error('rim_preset_key', t('A mapping already exists for that preset, please edit or delete the existing one.'));
    }
  }
  if ($preset_key !== 'none' && $mapping_1 == 'none' && $mapping_2 == 'none') {
    form_set_error('mapping_1', t('You must select at least one mapping.'));
  }
  if (($mapping_1 == $mapping_2) && ($mapping_2 == $preset_key) && ($preset_key !== 'none')) {
    form_set_error('rim_preset_key', t('Clever, but no need to map things to themselves. Mapping inception, aborted.'));
  }
}

/**
 * Submit handler for responsive_image_mapping_admin_form.
 *
 */
function responsive_image_mapping_admin_form_submit($form, &$form_state) {
  // Save the variables.
  // Eventually we can do a foreach on all breakpoints defined.
  $breakpoints = array(
    0 => $form_state['values']['rim_small_breakpoint'],
    1 => $form_state['values']['rim_medium_breakpoint'],
  );
  variable_set('rim_breakpoints', $breakpoints);
  variable_set('rim_image_default', $form_state['values']['rim_image_default']);
  // Save the mappings to database.
  if ($form_state['values']['rim_preset_key'] !== 'none') {
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
    drupal_set_message(t('Setting saved. Mapping created.'));
  }
  else {
    drupal_set_message(t('Setting saved. No mapping created, no preset key selected.'));
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
  $form['rim_add_mapping']['rim_preset_key'] = array(
    '#type' => 'select',
    '#title' => t('Preset Key'),
    '#options' => $style_names,
    '#description' => t('Choose the image preset to map. If no preset is chosen here, no mapping will be created.'),
    '#default_value' => variable_get('rim_preset_key', $tag['preset_key']),
  );
  $form['rim_add_mapping']['rim_mapping_1'] = array(
    '#type' => 'select',
    '#title' => t('Small Breakpoint Mapping'),
    '#options' => $style_names,
    '#description' => t('Choose the default preset to be used for small screens. Note this preset will be used for default if the option is chosen above.'),
    '#default_value' => variable_get('rim_mapping_1', $tag['mappings'][0]),
  );
  $form['rim_add_mapping']['rim_mapping_2'] = array(
    '#type' => 'select',
    '#title' => t('Medium Breakpoint'),
    '#options' => $style_names,
    '#description' => t('Choose the default preset to be used for medium screens. Viewports inbetween the breakpoints assigned above.'),
    '#default_value' => variable_get('rim_mapping_2', $tag['mappings'][1]),
  );
  $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Save mapping'),
  );
  $form['cancel'] = array(
   '#markup' => '<a href="admin/config/media/image-styles/responsive-mappings">' . t('Cancel') . '</a>',
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
    $mappings = array(
      0 => $form_state['values']['rim_mapping_1'],
      1 => $form_state['values']['rim_mapping_2'],
    );
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
