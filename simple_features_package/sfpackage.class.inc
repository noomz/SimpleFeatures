<?php
// $Id$

/**
 * @file
 * Package class, represent one package object.
 */

class Package {

  public $name;
  public $status;
  public $info;
  public $settings;

  /**
   * Helper function to load all available packages.
   *
   * @param $field
   *   Spacific field return field of packages.
   */
  public static function getInfoAll($field = NULL) {
    $package = array();
    $packages_path = drupal_get_path('module', 'sfpackage').'/packages/';
    foreach (glob($packages_path.'*.inc') as $package_path) {
      include_once($package_path);
      $package_name = substr(str_replace($packages_path, '', $package_path), 0, -4);
      $package = $package_name.'_package';
      $package = $package();
      $package['code'] = $package_name;
      if ($package['enable']) {
        $_package = (object) $package;
        if ($field) {
          foreach (explode(':', $field) as $depfield) {
            $_package = $_package[$depfield];
          }
        }
        $packages[$package_name] = $_package;
      }
    }

    return $packages;
  }

  /**
   * Wrapper function to load only wanted package.
   *
   * @param $package_name
   *   the machine of package to load
   */
  public static function getInfo($package_name) {
    $packages = Package::getInfoAll();
    if (array_key_exists($package_name, $packages)) {
      return $packages[$package_name];
    }
    else {
      return FALSE;
    }
  }

}
