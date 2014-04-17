<?php
/**
 * Clean up leftover fields / instances.
 */

/**
 * Clean up fields
 */
function net_reyero_cleanup_fields_page() {
  $build['form']  = drupal_get_form('net_reyero_cleanup_fields_form');
  return $build;
}

/**
 * Clean up fields / form
 */
function net_reyero_cleanup_fields_form($form, &$form_state) {
  if ($list = _net_reyero_cleanup_fields_list()) {
    $form['list'] = _net_reyero_cleanup_fields_table($list);
    $form['fields'] = array('#type' => 'value', '#value' => $list);
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Clean fields'));
  return $form;
}

/**
 * Clean up fields / form
 */
function net_reyero_cleanup_fields_form_submit($form, &$form_state) {
  if ($form_state['values']['fields']) {
    foreach ($form_state['values']['fields'] as $instance) {
      dpm($instance, 'deleting field instance');
      field_delete_instance($instance);
    }
  }
  // Purge data in the database.
  field_purge_batch(100);
}

/**
 * Get fields that can be cleaned up.
 */
function _net_reyero_cleanup_fields_list() {
  $list = &drupal_static(__FUNCTION__);
  if (!isset($list)) {
    $list = array();
    $instances = field_info_instances();
    $field_types = field_info_field_types();
    $bundles = field_info_bundles();
    foreach ($instances as $entity_type => $type_bundles) {
      foreach ($type_bundles as $bundle => $bundle_instances) {
        foreach ($bundle_instances as $field_name => $instance) {
          $field = field_info_field($field_name);

          if (!isset($bundles[$entity_type][$bundle])) {
            $list[] = $instance;
          }
        }
      }
    }
  }
  return $list;
}

/**
 * Get fields that can be cleaned up.
 */
function _net_reyero_cleanup_fields_table($list) {
  $header = array('Entity type', 'Bundle', 'Field name', 'Id', 'Data');
  $rows = array();

  foreach ($list as $instance) {
    $rows[] = array(
      $instance['entity_type'],
      $instance['bundle'],
      $instance['field_name'],
      $instance['id'],
      '',
    );
  }
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
}

