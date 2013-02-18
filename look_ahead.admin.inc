<?php
/**
 * @file
 * Administration page callbacks for the Menu Item View module.
 */

function _look_ahead_get_available_menus() {
  $menus = menu_get_menus();

  $exclude_menus = array();

  if (!empty($exclude_menus)) {
    $exclude_menus = array_combine($exclude_menus, $exclude_menus);
  }

  $available_menus = array_diff_key($menus, $exclude_menus);

  return $available_menus;
}

function _look_ahead_get_available_menu_blocks() {
  $active_menu_blocks = db_select('block', 'b')
    ->fields('b', array("title", "module", "delta"))
    ->condition('module', 'menu_block', '=')
    ->condition('status', '1', '=')
    ->distinct()
    ->execute()
    ->fetchAll();



  $available_menu_blocks = array();

  // manipulate array to pass in module name and delta to recognize in menu_link theme function
  foreach($active_menu_blocks as $menu_block){
    $available_menu_blocks["{$menu_block->module}:{$menu_block->delta}"] = variable_get("menu_block_{$menu_block->delta}_admin_title");
  }

  return $available_menu_blocks;
}

function _look_ahead_get_available_entity_types() {
  $entity_types = entity_get_info();
  $available_entity_types = array();

  $excluded_entity_types = array('entityform', 'file');

  foreach ($entity_types as $entity_type => $variables) {
    if (!$variables['fieldable']) {
      continue;
    }

    if (in_array($entity_type, $excluded_entity_types)) {
      continue;
    }

    $available_entity_types[$entity_type] = $variables['label'];
  }

  return $available_entity_types;
}

/**
 * Form builder. Configure Menu Item View
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function look_ahead_admin_settings() {
  $form = array();

  $form['look_ahead_entity_types_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled entity types'),
    '#description' => t('Enabled entity types will have a "Look-ahead" view mode and may have additional options'),
    '#options' => _look_ahead_get_available_entity_types(),
    '#default_value' => variable_get("look_ahead_entity_types_enabled", array()),
  );

  $form['look_ahead_menus_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled menus'),
    '#description' => t('Enabled menus will load look-ahead items if available'),
    '#options' => _look_ahead_get_available_menus(),
    '#default_value' => variable_get("look_ahead_menus_enabled", array()),
  );

  /*$form['look_ahead_menu_blocks_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable on menu blocks'),
    '#description' => t('Checking one or more menu blocks will only render look ahead items on those blocks'),
    '#options' => _look_ahead_get_available_menu_blocks(),
    '#default_value' => variable_get("look_ahead_menu_blocks_enabled", array()),
  );*/

  $form['look_ahead_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#description' => t('Configure how look-ahead items should work'),
    '#collapsible' => false,
    '#collapsed' => false,
  );

  $form['look_ahead_settings']['look_ahead_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable look-ahead functionality'),
    '#description' => t('If unchecked, look-ahead will be disabled site-wide.'),
    '#default_value' => variable_get('look_ahead_enabled', true),
  );

  $form['look_ahead_settings']['look_ahead_ajax'] = array(
    '#type' => 'checkbox',
    '#title' => t('Load look-ahead items with AJAX'),
    '#description' => t('Loads each look-ahead item at the time the parent item is hovered over.'),
    '#default_value' => variable_get('look_ahead_ajax', false),
  );

  $form['look_ahead_settings']['look_ahead_static_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache look-ahead items as static HTML'),
    '#description' => t('Only takes effect when loading items asynchronously'),
    '#default_value' => variable_get('look_ahead_static_cache'),
  );

  $form['look_ahead_settings']['look_ahead_static_cache_dir'] = array(
    '#type' => 'textfield',
    '#title' => 'Static cache directory',
    '#description' => t('If static cache is enabled, this is the directory under "files" where the cache files will be stored'),
    '#default_value' => variable_get('look_ahead_static_cache_dir', 'look_ahead_cache'),
  );

  $form['#submit'][] = 'look_ahead_admin_settings_submit';

  return system_settings_form($form);
}

function look_ahead_admin_settings_submit($form, &$form_state) {
  if (empty($form_state['values']['look_ahead_menus_enabled'])) {
    return;
  }

  $menus = array_filter($form_state['values']['look_ahead_menus_enabled']);

  foreach ($menus as $menu) {
    $links = menu_load_links($menu);

    foreach ($links as $link) {
      menu_link_save($link);
    }
  }
}
