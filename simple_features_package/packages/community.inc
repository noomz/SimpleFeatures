<?php

function community_package() {
  return array(
    'name' => 'Community',
    'description' => '',
    'thumbnail' => drupal_get_path('module', 'sfpackage').'/packages/community.jpg',
    'theme' => 'seven',
    'features' => array(
      'rgallery', 
    ),
    'enable' => TRUE,
  );
}
