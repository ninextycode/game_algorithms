import cytoscape, { type ElementDefinition } from 'cytoscape';
import fcose from 'cytoscape-fcose';
import graphStyles from './graph_style';      // styles moved to their own file

cytoscape.use(fcose);                         // register the layout plugin

/* ---------- 1. create an empty graph ---------- */
const cy = cytoscape({
  container: document.getElementById('cy') as HTMLElement,
  style: graphStyles,                         // externalised Cytoscape styles
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

      // JGF node
      if (item.id && !item.source && !item.target) {
        return {
          group: 'nodes',
          data: { id: item.id, label: item.label || item.id, ...item }
        };
      }

      // JGF edge
      if (item.source && item.target) {
        return {
          group: 'edges',
          data: {
            id: item.id || `${item.source}-${item.target}`,
            source: item.source,
            target: item.target,
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
            id: item.id,
            label: item.properties?.label ?? item.id,
            ...item.properties
          }
        };

      if (item.type === 'edge')
        return {
          group: 'edges',
          data: {
            id: item.id,
            source: item.outV,
            target: item.inV,
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
    const data = text.trim().includes('\n')
      ? text.trim().split('\n').map(line => JSON.parse(line))      // GraphSON lines
      : JSON.parse(text);                            // JGF or Cytoscape elements

    const elements = toElements(data);

    if (!elements.length) {
      alert('No valid nodes or edges found in the file.');
      return;
    }

    cy.elements().remove();
    cy.add(elements);
    cy.layout({ name: 'fcose' }).run();
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

/* ---------- 6. optional auto‑load test.json ---------- */
async function loadTestFile() {
  try {
    const r = await fetch('./test.json');
    if (r.ok) {
      const data = await r.json();
      const elements = toElements(data);
      if (elements.length) {
        cy.add(elements);
        cy.layout({ name: 'fcose' }).run();
      }
    }
  } catch {
    /* silently ignore if test.json isn’t present */
  }
}

document.addEventListener('DOMContentLoaded', loadTestFile);