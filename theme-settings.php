<?php
function ultimate_mobile_form_system_theme_settings_alter(&$form, $form_state) {

  $form['grid_settings'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Choose you settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

}
