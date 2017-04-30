<?php
/**
 * Clean up leftover fields / instances.
 */

/**
 * Clean up text formats
 */
function net_reyero_cleanup_formats_page($id = NULL) {

  if ($id && ($format = filter_format_load($id))) {
    drupal_set_title("Clean format: $id $format->name");
    $build['text']['#markup'] = '<p>' . "Clean fields with text format: $format->name" . '</p>';
    $build['form']  = drupal_get_form('net_reyero_cleanup_formats_form', $id);
  }
  else {
    // List text formats
    foreach (filter_formats() as $id => $format) {
      $list[$id] = l($format->name, 'net_reyero/cleanup/formats/' . $id);
    }
    $build['list'] = array(
      '#theme' => 'item_list',
      '#items' => $list,
    );
  }
  return $build;
}

/**
 * Clean up fields / form
 */
function net_reyero_cleanup_formats_form($form, &$form_state, $format_id) {
  $form['format'] = array('#type' => 'value', '#value' => $format_id);
  if ($list = net_reyero_cleanup_formats_fields_list($format_id)) {
    $form['select'] = net_reyero_cleanup_formats_fields_table($list);
    $form['fields'] = array('#type' => 'value', '#value' => $list);
    // Replace by format.
    $options = array();
    foreach (filter_formats() as $id => $format) {
      if ($id == $format_id) continue;
      $options[$id] = $format->name;
    }
    $form['replace'] = array(
      '#type' => 'select',
      '#title' => t('Replace by text format'),
      '#options' => $options,
    );
  }
  else {
    $form['message']['#markup'] = '<h4>No fields found with this format</h4>';
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Replace formats'));
  return $form;
}

/**
 * Clean up fields / form
 */
function net_reyero_cleanup_formats_form_submit($form, &$form_state) {

  $selected = array_filter($form_state['values']['select']);
  if ($selected) {
    $old_format = $form_state['values']['format'];
    $new_format = $form_state['values']['replace'];
    drupal_set_message("Updating " . count($selected) . " fields froum $old_format to $new_format");
    foreach ($selected as $name) {
      $field = $name . '_format';
      $tables = array('field_data_' . $name, 'field_revision_' . $name);
      foreach ($tables as $table) {
        $result = db_query("UPDATE {" . $table . "} SET $field = :new_format WHERE $field = :old_format", array(
          ':old_format' => $old_format,
          ':new_format' => $new_format,
        ));
        drupal_set_message("Replaced $table.$field");
      }
    }

  }
  else {
    drupal_set_message('No fields selected', 'warning');
  }

}

/**
 * Get fields that can be cleaned up.
 */
function net_reyero_cleanup_formats_fields_list($format_id) {
  $list = &drupal_static(__FUNCTION__);
  if (!isset($list)) {
    $list = array();
    $field_types = field_info_field_types();
    $fields = field_info_fields();
    //$bundles = field_info_bundles();
    foreach ($fields as $name => $info) {
      if (in_array($info['type'], array('text', 'text_long', 'text_with_summary'))) {
        $table = 'field_data_' . $name;
        $field = $name . '_format';
        $count = db_query("SELECT count(*) FROM {" . $table . "} WHERE $field = :format", array(
          ':format' => $format_id
        ))->fetchField();
        if ($count) {
          $info['label'] = $field_types[$info['type']]['label'];
          $info['count'] = $count;
          $list[$name] = $info;
        }
      }
    }
  }
  return $list;
}

/**
 * Get fields that can be cleaned up.
 */
function net_reyero_cleanup_formats_fields_table($list) {
  $header = array(
    'name' => 'Field',
    'label' => 'Label',
    'data' => 'Occurrences'
  );
  $options = array();

  foreach ($list as $name => $field) {
    $options[$name] = array(
      'name' => $name,
      'label' => $field['label'],
      'data' => $field['count'],
    );
  }
  return array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No content available.'),
  );
  return array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
  );
}


