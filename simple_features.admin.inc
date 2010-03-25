<?php
// $Id$

/**
 * @file
 */

/**
 * Settings form.
 */
function simple_features_list_form() {
  $features_info = Feature::getInfoAll();
  ksort($features_info);
  $form = array();

  $form['package'] = array(
    '#type' => 'vertical_tabs',
  );

  foreach ($features_info as $package => $modules) {
    $pkg_name = 'sf_pkg_'. $package;

    $form[$pkg_name] = array(
      '#type' => 'fieldset',
      '#title' => $package,
      '#collapsible' => TRUE,
      '#group' => 'package',
    );

    $implemented_modules = module_implements('sf_settings');
    foreach ($modules as $module) {
      $mod_name = 'sf_mod_'. $module->name;
      $form[$pkg_name][$module->name] = array(
        '#type' => 'fieldset',
        '#tree' => TRUE,
      );

      $form[$pkg_name][$module->name]['status'] = array(
        '#type' => 'checkbox',
        '#title' => $module->info['description'] .' ['. $module->info['name'] .']',
        '#tree' => TRUE,
        '#default_value' => $module->status,
      );

      if ($module->status == 1 && in_array($module->name, $implemented_modules)) {
        $form[$pkg_name][$module->name]['settings'] = array(
          '#type' => 'fieldset',
          '#title' => 'Settings',
          '#prefix' => '<div class="sf_mod_settings_subform_wrapper" id="'. $mod_name .'_subform">',
          '#suffix' => '</div>',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#tree' => TRUE,
        );

        // Get feature settings form.
        $feature = new Feature($module->name);
        $subforms = module_invoke($module->name, 'sf_settings');
        foreach ($subforms as $key => &$subform) {
          $subform['#is_feature_field'] = TRUE;
          $subform['#default_value'] = $feature->get_settings($key);
        }
        $form[$pkg_name][$module->name]['settings'] = array_merge($form[$pkg_name][$module->name]['settings'], $subforms);
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';

  return $form;
}

/**
 * Handle form submission.
 */
function simple_features_list_form_submit($form, &$form_state) {
  // Filter only features form.
  $values = array_filter($form_state['values'], create_function('$v',
    'return is_array($v);'));

  $enabled = $disabled = array();
  foreach ($values as $key => $value) {
    $feature = new Feature($key);
    if ($feature->status != $value['status']) {
      if ($value['status'] == 1) {
        $enabled[] = $key;
        continue;
      }
      else {
        $disabled[] = $key;
        $disabled = array_merge($disabled, $feature->info->info['dependencies']);
      }
    }
    if (array_key_exists('settings', $value)) {
      $feature->settings = $value['settings'];
      $feature->save_settings();
      module_invoke($key, 'sf_settings_submit', $value['settings']);
    }
  }

  module_disable($disabled);
  module_enable($enabled);
  simple_features_set_default_variables($enabled);
  if (!empty($disabled) || !empty($enabled)) {
    drupal_flush_all_caches();
  }
}
