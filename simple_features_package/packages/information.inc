<?php

function information_package() {
  return array(
    'name' => 'Information',
    'description' => '',
    'thumbnail' => drupal_get_path('module', 'sfpackage').'/packages/information.jpg',
    'theme' => 'seven',
    'features' => array(
      'blog',
      'contact',
    ),
    'enable' => TRUE,
  );
}
