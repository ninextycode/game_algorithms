import type { StylesheetCSS } from 'cytoscape';

/**
 * Central place for node / edge styling.
 * Import this constant wherever you need Cytoscape styles.
 */
const graphStyles: StylesheetCSS[] = [
  {
    selector: 'node',
    css: {
      'background-color': '#0074D9',
      'label': 'data(id)',
      'text-valign': 'center',
      'color': '#fff',
      'font-size': 14,
      'font-weight': 'bold',
      'width': 40,
      'height': 40,
      'text-outline-width': 2,
      'text-outline-color': '#000'
    }
  },
  {
    selector: 'edge',
    css: {
      'width': 3,
      'line-color': '#666',
      'target-arrow-color': '#666',
      'label': 'data(label)',
      'font-size': 10,
      'curve-style': 'bezier',
      'text-background-color': '#fff',
      'text-background-opacity': 0.8,
      'text-background-padding': '3px'
    }
  },
  {
    /* directed = edge[directed = true] or edge[?directed] */
    selector: 'edge[?directed]',
    css: { 'target-arrow-shape': 'triangle' }
  }
];

export default graphStyles;