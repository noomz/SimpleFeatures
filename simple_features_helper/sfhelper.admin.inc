<?php
// $Id$

/**
 * @file
 */
function sfhelper_export($type = NULL) {
  $node_types = node_type_get_types();
  ksort($node_types);
  $output = theme('sfhelper_node_type_list', array('node_types' => $node_types));

  if ($type) {
    $output .= '<div class="sfhelper-code" id="sfhelper-type-code-raw">';
    $output .= '<h3>Print by print_r</h3>';
    $output .= '<pre>'. check_plain(print_r($type, 1)) .'</pre>';
    $output .= '</div>';

    $output .= '<div class="sfhelper-code" id="sfhelper-type-code-serialized">';
    $output .= '<h3>Serialized Object</h3>';
    $output .= '<textarea rows="5" cols="100">';
    $output .= serialize($type);
    $output .= '</textarea>';
    $output .= '</div>';
  }

  return $output;
}

/**
 * Theme function.
 */
function theme_sfhelper_node_type_list($variables) {
  $node_types = $variables['node_types'];
  $output = '<ul class="sfhelper-node-type-list">';
  foreach ($node_types as $type => $info) {
    $output .= theme('sfhelper_node_type_item', array('info' => $info));
  }
  $output .= '</ul>';

  return $output;
}

/**
 * Theme function.
 */
function theme_sfhelper_node_type_item($variables) {
  $info = $variables['info'];
  $output = '<li class="sfhelper-node-type-item">';
  $output .= '  <div class="sfhelper-node-type-box">';
  $output .= $info->name .' ( ';
  $output .= l('export', 'admin/features/export/'. $info->type);
  $output .= ' ) ';
  $output .= '  </div>';
  $output .= '</li>';

  return $output;
}
