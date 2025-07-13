import type { StylesheetCSS } from 'cytoscape';

/**
 * Central place for node / edge styling.
 * Import this constant wherever you need Cytoscape styles.
 */
const graphStyles: StylesheetCSS[] = [
  {
    selector: 'node',
    css: {
      'background-color': '#5DADE2',
      'label': 'data(label)',
      'text-valign': 'center',
      'text-halign': 'center',
      'color': '#000000',
      'font-size': 8,
      'font-family': 'monospace',
      'width': 'label',
      'height': 'label',
      'shape': 'rectangle',
      'text-wrap': 'wrap',
      'text-max-width': '160px',
      'padding': '4px',
      'text-margin-y': 2
    }
  },
  {
    // Game: Terminal nodes
    selector: 'node[nodeType = "Terminal"]',
    css: {
      'background-color': '#F1948A',
    }
  },
  {
    // Game: Chance nodes
    selector: 'node[nodeType = "Chance"]',
    css: {
      'background-color': '#F7DC6F',
    }
  },
  {
    // Game: Decision nodes
    selector: 'node[nodeType = "Decision"]',
    css: {
      'background-color': '#82E0AA',
    }
  },
  {
    selector: 'edge',
    css: {
      'width': 2,
      'line-color': '#666',
      'target-arrow-color': '#666',
      'target-arrow-shape': 'triangle',
      'label': 'data(label)',
      'font-size': 8,
      'curve-style': 'bezier',
      'text-background-color': '#fff',
      'text-background-opacity': 0.9,
      'text-background-padding': '2px',
      'text-rotation': 'none'
    }
  },
  {
    // Game: Action edges with different styling
    selector: 'edge[action]',
    css: {
      'line-color': '#333',
      'width': 3,
      'target-arrow-color': '#333'
    }
  },
  {
    /* directed = edge[directed = true] or edge[?directed] */
    selector: 'edge[?directed]',
    css: { 'target-arrow-shape': 'triangle' }
  }
];

export default graphStyles;