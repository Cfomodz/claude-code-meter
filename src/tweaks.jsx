function Tweaks({ state, setState }) {
  const [open, setOpen] = React.useState(false);
  const [enabled, setEnabled] = React.useState(false);

  React.useEffect(() => {
    const onMsg = (e) => {
      if (!e.data) return;
      if (e.data.type === '__activate_edit_mode') { setEnabled(true); setOpen(true); }
      if (e.data.type === '__deactivate_edit_mode') { setEnabled(false); setOpen(false); }
    };
    window.addEventListener('message', onMsg);
    window.parent.postMessage({ type: '__edit_mode_available' }, '*');
    return () => window.removeEventListener('message', onMsg);
  }, []);

  const update = (patch) => {
    const next = { ...state, ...patch };
    setState(next);
    window.parent.postMessage({ type: '__edit_mode_set_keys', edits: patch }, '*');
  };

  if (!enabled) return null;
  return (
    <>
      <button className="tw-toggle" onClick={()=>setOpen(!open)}>
        {open ? '× Tweaks' : '◆ Tweaks'}
      </button>
      {open && (
        <div className="tw-panel">
          <h4>TWEAKS</h4>
          <div className="tw-row">
            <label>ACCENT</label>
            <div className="tw-swatches">
              {['amber','green','cyan','red'].map(c => (
                <button key={c} className={state.accent===c?'on':''}
                  style={{'--c': c==='amber'?'#e8a020':c==='green'?'#6ab04a':c==='cyan'?'#4bb8c7':'#d94b3a'}}
                  onClick={()=>update({accent:c})} title={c}/>
              ))}
            </div>
          </div>
          <div className="tw-row">
            <label>HERO METRIC</label>
            <div className="tw-seg">
              {['SESS','WEEK','API'].map((l,i) => (
                <button key={l} className={state.metric===i?'on':''} onClick={()=>update({metric:i})}>{l}</button>
              ))}
            </div>
          </div>
          <div className="tw-row">
            <label>GRID OVERLAY</label>
            <div className="tw-seg">
              {['ON','OFF'].map(l => (
                <button key={l} className={(state.grid==='on'?'ON':'OFF')===l?'on':''}
                  onClick={()=>update({grid: l==='ON'?'on':'off'})}>{l}</button>
              ))}
            </div>
          </div>
        </div>
      )}
    </>
  );
}
Object.assign(window, { Tweaks });
