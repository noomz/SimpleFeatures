<?php
// $Id$

/**
 * @file
 */

/**
 * Features list form.
 * User can turn on or off features on this page.
 */
function simple_features_features_list_form() {
  $form = array();

  $features = simple_features_get_all_features();
  $simple_features_list = variable_get('simple_features_list', array());
  $form['simple_features_list'] = array(
    '#type' => 'form',
    '#tree' => TRUE,
  );
  foreach ($features as $key => $feature) {
    $form['simple_features_list'][$key] = array(
      '#type' => 'checkbox',
      '#title' => $feature['description'],
      '#default_value' => $simple_features_list[$key],
      '#suffix' => l("Configure...", $feature['config'], array('query' => array('destination' => 'admin/config/system/features_list'))),
    );
  }

  $form['#submit'][] = 'simple_features_features_list_form_submit';

  return system_settings_form($form);
}

/**
 * Submit handler.
 */
function simple_features_features_list_form_submit($form, &$form_state) {
  $configs = simple_features_get_all_features();
  
  $modules = array(0 => array(), 1 => array());
  $features_list = $form_state['values']['simple_features_list'];
  foreach ($features_list as $key => $feature) {
    $status = (int)!empty($feature);
    $modules[$status] = array_merge(
        $modules[$status],
        $configs[$key]['dependencies']
    );
  }

  $already_enabled = variable_get('simple_features_list', array());
  $will_config = array_diff($features_list, $already_enabled);

  // Extract module that will be enabled from disabled list.
  $disabled = array_diff($modules[0], $modules[1]);
  $enabled = $modules[1];
  module_disable($disabled);
  module_enable($enabled);
  simple_features_auto_config($will_config);
  drupal_flush_all_caches();
}
