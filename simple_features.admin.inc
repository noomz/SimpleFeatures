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
        '#title' => $module->info['name'],
        '#description' => $module->info['description'],
        '#tree' => TRUE,
        '#default_value' => $module->status,
        '#theme_wrappers' => array('simple_features_checkbox_element'),
      );

      // Check for preview image, use default if not provides
      if (!$module->info['preview_image']) {
        $path = drupal_get_path('module', 'simple_features') . '/preview-default.png';
      }
      else {
        $path = drupal_get_path('module', $module->name) .'/'. $module->info['preview_image'];
      }
      $form[$pkg_name][$module->name]['status']['#preview_image'] = $path;

      // Add price information
      $form[$pkg_name][$module->name]['status']['#price'] = $module->info['price'];

      // Add sample link if module is enabled.
      $sample_contents = module_invoke($module->name, 'sf_sample_contents');
      foreach ($sample_contents as $content) {
        if (isset($content['#link'])) {
          $form[$pkg_name][$module->name]['status']['#sample_links'][] = $content['#link'];
        }
      }

      // If module is enabled, show settings form.
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
  $form['#attached']['css'][] = drupal_get_path('module', 'simple_features') . '/simple_features.css';

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
        $enabled = array_merge($enabled, $feature->info->info['dependencies']);
        continue;
      }
      else {
        $disabled[] = $key;
        // Disable all dependencied modules, then blog will not be use if
        // features_blog.module is disabled.
        // TODO: Option to disable dependencies or not, Or provide new
        // TODO: configuration 'strong_dependencies', FYI.
        $disabled = array_merge($disabled, $feature->info->info['dependencies']);
      }
    }
    if (array_key_exists('settings', $value)) {
      $feature->settings = $value['settings'];
      module_invoke($key, 'sf_settings_submit', $value['settings'], $feature);
      $feature->save_settings();
    }
  }

  // TODO: Check if this approach really works.
  $clear_cache = !empty($disabled) || !empty($enabled);
  _txn('simple_features_disable_features' ,$disabled, FALSE);
  _txn('simple_features_enable_features', $enabled, $clear_cache);
}

/**
 * Theme form element.
 * Override default theme_checkbox_element.
 */
function theme_simple_features_checkbox_element($variables) {
  $element = $variables['element'];
  // This is also used in the installer, pre-database setup.
  $t = get_t();

  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $class = array('form-item');
  if (!empty($element['#type'])) {
    $class[] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $class[] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }

  $output = '<div class="' . implode(' ', $class) . '">' . "\n";

  $output .= ' <div class="features-checkbox">';
  $output .= ' ' . $element['#children'];
  $output .= " </div>\n";

  // Add preview image.
  $output .= theme('simple_features_preview_image', array('path' => $element['#preview_image'], 'width' => 194, 'height' => 144));

  // Add more details.
  $output .= '  <div class="features-detail">';
  $output .= '  ' . theme('form_element_label', $variables) . "\n";

  if (!empty($element['#description'])) {
    $output .= '  <div class="description">' . $element['#description'] . "</div>\n";
  }

  // Print price.
  $output .= theme('simple_features_price', array('price' => $element['#price']));

  // Sample content links.
  $output .= theme('simple_features_sample_links', array('links' => $element['#sample_links']));

  $output .= "  </div>\n";
  $output .= "</div>\n";

  return $output;
}

/**
 * Theme preview image.
 */
function theme_simple_features_preview_image($variables) {
  $path = $variables['path'];
  $width = $variables['width'];
  $height = $variables['height'];

  $output = '<div class="features-preview">';
  $output .= theme('image', array('path' => $path, 'attributes' => array('width' => $width, 'height' => $height), 'getsize' => FALSE));
  $output .= "</div>\n";

  return $output;
}

/**
 * Theme price.
 */
function theme_simple_features_price($variables) {
  $price = $variables['price'];
  $output = '<div class="features-price">';
  $output .= '<span class="features-price-label">Price : </span>';
  $output .= '<span class="features-price-value">';
  $output .= $price ? $price . ' Baht' : 'FREE' ;
  $output .= "</span></div>\n";

  return $output;
}

/**
 * Theme sample links.
 */
function theme_simple_features_sample_links($variables) {
  $links = $variables['links'];
  $output = '';
  if (!empty($links)) {
    $output .= '<div class="features-links">';
    $output .= '<h4>Content Sample Links</h4>';
    $output .= '<ul>';
    foreach ($links as $link) {
      $output .= '<li>' . $link . '</li>';
    }
    $output .= '</ul>';
    $output .= "</div>\n";
  }

  return $output;
}
