<?php

function blog_package() {
  return array(
    'name' => 'Blog',
    'description' => '',
    'thumbnail' => drupal_get_path('module', 'sfpackage').'/packages/blog.jpg',
    'theme' => 'seven', 
    'features' => array(
      'blog',
    ),
    'enable' => TRUE,
  );
}
