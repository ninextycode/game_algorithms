import cytoscape, { type ElementDefinition } from 'cytoscape';
import fcose from 'cytoscape-fcose';
import dagre from 'cytoscape-dagre';
import graphStyles from './graph_style';      // styles moved to their own file

cytoscape.use(fcose);                         // register the layout plugin
cytoscape.use(dagre);                         // register the dagre layout plugin

/* ---------- Helper function for left-aligned text formatting ---------- */
function formatLeftAligned(text: string): string {
  if (!text) return text;
  
  const lines = text.split('\n');
  if (lines.length <= 1) return text;
  
  // Find the maximum line length
  const maxLength = Math.max(...lines.map(line => line.length));
  
  // Pad each line to match the maximum length
  const paddedLines = lines.map(line => {
    return '|' + line.padEnd(maxLength, ' ') + '|';
  });
  
  return paddedLines.join('\n');
}

/* ---------- Tree layout configuration ---------- */
const treeLayout = {
  name: 'dagre',
  directed: true,
  padding: 30,
  spacingFactor: 1.0,
  animate: true,
  animationDuration: 500,
  rankDir: 'TB', // Top to Bottom
  ranker: 'network-simplex',
  nodeSep: 30,    // Horizontal spacing between nodes at same level
  rankSep: 20     // Vertical spacing between levels (reduced from 100)
};

/* ---------- 1. create an empty graph ---------- */
const cy = cytoscape({
  container: document.getElementById('cy') as HTMLElement,
  style: graphStyles,                         // externalized Cytoscape styles
  layout: { name: 'grid' }                    // placeholder until data arrives
});

/* ---------- 2. convert incoming JSON → Cytoscape elements ---------- */
function toElements(data: any): ElementDefinition[] {
  let rows: any[] = [];

  if (Array.isArray(data)) {
    rows = data;                              // already an array
  } else if (data.nodes && data.edges) {
    rows = [...data.nodes, ...data.edges];    // JGF snapshot
  }

  return rows
    .map(item => {
      // Already Cytoscape‑shaped
      if (item.group === 'nodes' || item.group === 'edges') return item;

      // Game Tree node (has type, representation, address)
      if (item.type && item.representation !== undefined && item.id !== undefined && !item.source && !item.target) {
        return {
          group: 'nodes',
          data: { 
            id: item.id.toString(), 
            label: formatLeftAligned(item.representation),
            nodeType: item.type,
            address: item.address,
            ...item
          }
        };
      }

      // Game Tree edge (has source, target, action, action_string)
      if (item.source !== undefined && item.target !== undefined && item.action !== undefined) {
        return {
          group: 'edges',
          data: {
            id: item.id || `${item.source}-${item.target}`,
            source: item.source.toString(),
            target: item.target.toString(),
            label: item.action_string || item.action.toString(),
            action: item.action,
            action_string: item.action_string
          }
        };
      }

      // JGF node
      if (item.id !== undefined && !item.source && !item.target) {
        return {
          group: 'nodes',
          data: { 
            id: item.id.toString(), 
            label: formatLeftAligned(item.label || item.id.toString()), 
            ...item 
          }
        };
      }

      // JGF edge
      if (item.source && item.target) {
        return {
          group: 'edges',
          data: {
            id: item.id || `${item.source}-${item.target}`,
            source: item.source.toString(),
            target: item.target.toString(),
            label: item.label || '',
            ...item
          }
        };
      }

      // GraphSON node / edge
      if (item.type === 'node')
        return {
          group: 'nodes',
          data: {
            id: item.id.toString(),
            label: formatLeftAligned(item.properties?.label ?? item.id.toString()),
            ...item.properties
          }
        };

      if (item.type === 'edge')
        return {
          group: 'edges',
          data: {
            id: item.id,
            source: item.outV.toString(),
            target: item.inV.toString(),
            label: item.label
          }
        };
    })
    .filter(Boolean) as ElementDefinition[];
}

/* ---------- 3. load a selected or dropped file ---------- */
async function loadFile(file: File) {
  try {
    const text = await file.text();
    
    // Try to parse as regular JSON first
    let data;
    try {
      data = JSON.parse(text);
    } catch {
      // If that fails, try parsing as GraphSON (line-by-line JSON)
      data = text.trim().split('\n').map(line => JSON.parse(line));
    }

    const elements = toElements(data);

    if (!elements.length) {
      alert('No valid nodes or edges found in the file.');
      return;
    }

    cy.elements().remove();
    cy.add(elements);
    
    // Debug: Log the elements to see what we're working with
    console.log('Loaded elements:', elements);
    console.log('Nodes:', elements.filter(e => e.group === 'nodes'));
    console.log('Edges:', elements.filter(e => e.group === 'edges'));
    
    // Use hierarchical layout for tree structure (top to bottom)
    cy.layout(treeLayout).run();
  } catch (err: any) {
    alert(`Could not parse file: ${err.message}`);
  }
}

/* ---------- 4. file‑picker ---------- */
const picker = document.getElementById('filePicker') as HTMLInputElement;

picker.addEventListener('change', e => {
  const file = (e.target as HTMLInputElement).files?.[0];
  if (file) loadFile(file);
  picker.value = '';                            // allow re‑selecting the same file
});

/* ---------- 5. drag‑and‑drop ---------- */
['dragenter', 'dragover'].forEach(evt =>
  document.addEventListener(evt, e => {
    e.preventDefault();
    document.body.classList.add('dragover');
  })
);

['dragleave', 'drop'].forEach(evt =>
  document.addEventListener(evt, () => document.body.classList.remove('dragover'))
);

document.addEventListener('drop', e => {
  e.preventDefault();
  const file = (e as DragEvent).dataTransfer?.files?.[0];
  if (file) loadFile(file);
});

/* ---------- 6. optional auto‑load poker_graph.json ---------- */
async function loadTestFile() {
  try {
    const r = await fetch('./poker_graph.json');
    if (r.ok) {
      const data = await r.json();
      const elements = toElements(data);
      if (elements.length) {
        cy.add(elements);
        
        // Use hierarchical layout for tree structure (top to bottom)
        cy.layout(treeLayout).run();
      }
    }
  } catch {
    /* silently ignore if poker_graph.json isn’t present */
  }
}

document.addEventListener('DOMContentLoaded', loadTestFile);