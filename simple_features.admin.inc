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
  $options = array();
  foreach ($features as $feature) {
    $options[$feature['name']] = $feature['description'];
  }

  $form['simple_features_list'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Features list'),
    '#description' => t('Select which features you want to use.'),
    '#options' => $options,
    '#default_value' => variable_get('simple_features_list', array()),
  );

  $form['#submit'][] = 'simple_features_features_list_form_submit';

  return system_settings_form($form);
}

/**
 * Submit handler.
 */
function simple_features_features_list_form_submit($form, &$form_state) {
  $configs = simple_features_get_all_features();
  $features_configs = array();
  foreach ($configs as $config) {
    $features_configs[$config['name']] = $config;
  }

  $modules = array(0 => array(), 1 => array());
  foreach ($form_state['values']['simple_features_list'] as $key => $feature) {
    $status = (int)!empty($feature);
    $modules[$status] = array_merge(
        $modules[$status],
        $features_configs[$key]['dependencies']
    );
  }

  // Extract module that will be enabled from disabled list.
  $disabled = array_diff($modules[0], $modules[1]);
  $enabled = $modules[1];
  module_disable($disabled);
  module_enable($enabled);
  simple_features_auto_fill_form($enabled);
  drupal_flush_all_caches();
}

/**
 * Format form element name.
 */
function format_form_element_name($name) {
  return strtr($name, array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
}

/**
 * Return unformat form element name.
 */
function unformat_form_element_name($name) {
  // TODO:
  return strtr($name, array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
}
