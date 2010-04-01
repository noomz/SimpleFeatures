<?php
// $Id$

/**
 * @file
 * Feature class, represent one feature object.
 */

class Feature {

  public $name;
  public $status;
  public $info;
  public $settings;

  function __construct($feature_name) {
    $this->name = $feature_name;
    $this->info = Feature::getInfo($feature_name);
    $this->status = $this->info->status;
    $settings = db_query("
      SELECT name, value
        FROM {sf_settings}
        WHERE feature = :feature_name LIMIT 1",
      array(':feature_name' => $feature_name)
    )
    ->fetchAll();
    // Make default empty settings as array.
    $this->settings = array();
    foreach ($settings as $value) {
      $this->settings[$value->name] = $value->value;
    }
  }

  /**
   * Return settings value or empty string if not exist.
   */
  function get_settings($key) {
    if (array_key_exists($key, $this->settings)) {
      return $this->settings[$key];
    }
    else {
      return FALSE;
    }
  }

  /**
   * Save settings values to database.
   */
  function save_settings() {
    if (empty($this->settings)) {
      return TRUE;
    }

    // As db_merge will be supported multiple values in drupal 8,
    // we need to delete all old settings before inserting new values.
    // By using db_insert multiple values method, I don't sure if it
    // increase preformance or not. (May be this help when values to
    // save is more than 100)
    // TODO: revise the method again.
    $txn = db_transaction();
    try {
      // Clear all old values.
      db_delete('sf_settings')
        ->condition('feature', $this->name)
        ->execute();

      // Begin insertion.
      $query = db_insert('sf_settings')->fields(array('feature', 'name', 'value'));
      foreach ($this->settings as $name => $value) {
        $query->values(array(
          'feature' => $this->name,
          'name' => $name,
          'value' => $value,
        ));
      }
      $query->execute();
      return TRUE;
    }
    catch (Exception $e) {
      $txn->rollback();
      return FALSE;
    }
  }

  /**
   * Helper function to load all available features.
   *
   * @param $group
   *   if TRUE return features group by category(package)
   */
  public static function getInfoAll($group = TRUE, $field = NULL) {
    $features = array();
    $modules = db_query("SELECT * FROM {system} WHERE type = 'module'")->fetchAll();
    foreach ($modules as &$module) {
      $module->info = unserialize($module->info);
      if (@$module->info['parent'] == 'simple_features') {
        
        // Expland to depthest field, split by :
        $_module = $module;
        if ($field) {
          foreach (explode(':', $field) as $depfield) {
            $_module = (array) $_module;
            $_module = $_module[$depfield];
          }
        }

        if ($group) {
          $features[$module->info['package']][] = $_module;
        }
        else {
          $features[$module->name] = $_module;
        }
      }
    }

    return $features;
  }

  /**
   * Wrapper function to load only wanted feature.
   *
   * @param $feature_name
   *   the machine of feature to load
   */
  public static function getInfo($feature_name) {
    $features = Feature::getInfoAll(FALSE);
    if (array_key_exists($feature_name, $features)) {
      return $features[$feature_name];
    }
    else {
      return FALSE;
    }
  }

}
