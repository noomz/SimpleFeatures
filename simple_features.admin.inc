<?php
// $Id$

/**
 * @file
 */

/**
 * Settings form.
 */
function simple_features_list_form() {
  $features_info = _load_features();
  $form = array();

  foreach ($features_info as $package => $modules) {
    $pkg_name = 'sf_pkg_'. $package;
    $form[$pkg_name] = array(
      '#type' => 'fieldset',
      '#title' => $package,
      '#collapsible' => TRUE,
    );
    foreach ($modules as $module) {
      $mod_name = 'sf_mod_'. $module->name;
      $form[$pkg_name][$module->name] = array(
        '#type' => 'form',
        '#prefix' => '<div class="sf_mod_settings_wrapper" id="'. $mod_name .'">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
      );

      $form[$pkg_name][$module->name]['status'] = array(
        '#type' => 'checkbox',
        '#title' => $module->info['description'] .' ['. $module->info['name'] .']',
        '#tree' => TRUE,
      );

      $form[$pkg_name][$module->name]['settings'] = array(
        '#type' => 'form',
        '#prefix' => '<div class="sf_mod_settings_subform_wrapper" id="'. $mod_name .'_subform">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
      );
      // Get feature settings form.
      $subforms = module_invoke($module->name, 'sf_settings');
      $feature = new Feature($module->name);
      foreach ($subforms as $key => &$subform) {
        $subform['#is_feature_field'] = TRUE;
        $subform['#default_value'] = $feature->get_settings($key);
      }
      $form[$pkg_name][$module->name]['settings'] = array_merge($form[$pkg_name][$module->name]['settings'], $subforms);
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  return $form;
}

/**
 * Handle form submission.
 */
function simple_features_list_form_submit($form, &$form_state) {
  // Filter only features form.
  $values = array_filter($form_state['values'], create_function('$v',
    'return is_array($v);'));

  foreach ($values as $key => $value) {
    $feature = new Feature($key);
    $feature->settings = $value['settings'];
    $feature->save_settings();
  }
}
