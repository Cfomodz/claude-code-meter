// Device mockup — a tiny horizontal bar display
// Shows 7-seg digits + pixel bar. Tap to cycle metrics.

const METRICS = [
  { id: 'session', label: 'SESS',  full: '5-HOUR SESSION',   value: 62, reset: '2H 14M', unit: '% LEFT',  tone: 'ok' },
  { id: 'weekly',  label: 'WEEK',  full: '7-DAY WINDOW',     value: 38, reset: '4D 02H', unit: '% LEFT',  tone: 'warn' },
  { id: 'api',     label: 'API ', full: 'API OVERAGE',       value:  4, reset: 'PLAN',   unit: '$ OVER',  tone: 'hot' },
];

// 7-segment digit. seg bitmap per digit.
const SEGS = {
  '0':'abcdef','1':'bc','2':'abdeg','3':'abcdg','4':'bcfg','5':'acdfg',
  '6':'acdefg','7':'abc','8':'abcdefg','9':'abcdfg','-':'g',' ':'','.':'.'
};
function SevenSeg({ digit, size = 30, on = '#e8a020', off = 'transparent', glow = true }) {
  const segs = SEGS[digit] || '';
  const w = size, h = size * 1.7, t = Math.max(2, size * 0.12); // thickness
  const pad = t * 0.6;
  const style = (s) => ({
    position:'absolute', background: segs.includes(s) ? on : off,
    boxShadow: glow && segs.includes(s) ? `0 0 ${size*0.3}px ${on}aa` : 'none',
    transition: 'background 0.2s',
  });
  const segBox = {
    a:{ left: pad, right: pad, top: 0, height: t },                                // top
    d:{ left: pad, right: pad, bottom: 0, height: t },                             // bottom
    g:{ left: pad, right: pad, top: '50%', height: t, transform:'translateY(-50%)' },
    b:{ right: 0, top: pad, bottom: '50%', width: t, marginBottom: t/2 },          // top-right
    c:{ right: 0, top: '50%', bottom: pad, width: t, marginTop: t/2 },             // bottom-right
    f:{ left: 0, top: pad, bottom: '50%', width: t, marginBottom: t/2 },           // top-left
    e:{ left: 0, top: '50%', bottom: pad, width: t, marginTop: t/2 },              // bottom-left
  };
  return (
    <div style={{ position:'relative', width: w, height: h, display:'inline-block', margin: '0 2px' }}>
      {Object.keys(segBox).map(s => <div key={s} style={{...segBox[s], ...style(s)}}/>)}
    </div>
  );
}

function PixelBar({ value, segments = 18, color = '#e8a020', height = 18 }) {
  const filled = Math.round((value/100)*segments);
  return (
    <div style={{display:'flex', gap: 2, alignItems:'center'}}>
      {Array.from({length: segments}).map((_,i) => (
        <div key={i} style={{
          width: 8, height,
          background: i < filled ? color : `${color}22`,
          boxShadow: i < filled ? `0 0 4px ${color}99` : 'none',
          transition:'background 0.3s',
        }}/>
      ))}
    </div>
  );
}

// the device itself. mode: 'live' | 'wireframe' | 'exploded'
function Device({ metric, mode = 'live', scale = 1, showAnnotations = false }) {
  const m = METRICS[metric % METRICS.length];
  const toneColor = m.tone === 'ok' ? '#6ab04a' : m.tone === 'warn' ? '#e8a020' : '#d94b3a';

  const valStr = String(m.value).padStart(2,' ');

  // Overall body dims (in design px — scaled by transform)
  const W = 420, H = 130;

  if (mode === 'wireframe') {
    return (
      <div style={{
        width: W*scale, height: H*scale,
        background:'#0a0f14', border:'1px solid #4bb8c7', position:'relative',
        fontFamily: 'var(--mono)',
      }}>
        <svg viewBox={`0 0 ${W} ${H}`} width="100%" height="100%">
          <defs>
            <pattern id="grid" width="10" height="10" patternUnits="userSpaceOnUse">
              <path d="M 10 0 L 0 0 0 10" fill="none" stroke="#4bb8c722" strokeWidth="0.5"/>
            </pattern>
          </defs>
          <rect width={W} height={H} fill="url(#grid)"/>
          {/* outer body */}
          <rect x="10" y="10" width={W-20} height={H-20} fill="none" stroke="#4bb8c7" strokeWidth="1" rx="8"/>
          {/* display window */}
          <rect x="28" y="30" width={W-56} height={H-60} fill="none" stroke="#4bb8c7" strokeDasharray="3 2" rx="2"/>
          {/* button */}
          <circle cx={W-28} cy={H/2} r="6" fill="none" stroke="#4bb8c7"/>
          <text x={W-28} y={H/2 + 22} textAnchor="middle" fill="#4bb8c7" fontSize="7" fontFamily="monospace">BTN</text>
          {/* dimensions */}
          <g stroke="#4bb8c7" strokeWidth="0.5" fill="#4bb8c7" fontSize="8" fontFamily="monospace">
            <line x1="10" y1={H-5} x2={W-10} y2={H-5}/>
            <line x1="10" y1={H-7} x2="10" y2={H-3}/>
            <line x1={W-10} y1={H-7} x2={W-10} y2={H-3}/>
            <text x={W/2} y={H-8} textAnchor="middle">3.12"  (79mm)</text>

            <line x1={W-5} y1="10" x2={W-5} y2={H-10}/>
            <line x1={W-7} y1="10" x2={W-3} y2="10"/>
            <line x1={W-7} y1={H-10} x2={W-3} y2={H-10}/>
            <text x={W-10} y={H/2} textAnchor="end" transform={`rotate(-90 ${W-10} ${H/2})`}>0.97"</text>
          </g>
          <text x="14" y="22" fill="#4bb8c7" fontSize="7" fontFamily="monospace" letterSpacing="1">LIMITBAR v0.3 · TOP VIEW</text>
          <text x={W-14} y="22" textAnchor="end" fill="#4bb8c7" fontSize="7" fontFamily="monospace" letterSpacing="1">SHEET 01/04</text>
        </svg>
      </div>
    );
  }

  // LIVE mode
  return (
    <div style={{
      position:'relative',
      width: W*scale, height: H*scale,
      transformOrigin:'top left',
    }}>
      <div style={{
        width: W, height: H, position:'relative',
        transform: `scale(${scale})`, transformOrigin: 'top left',
        background: 'linear-gradient(180deg, #2a2520 0%, #1a1713 60%, #0f0d0a 100%)',
        borderRadius: 12,
        border: '1px solid #3a3528',
        boxShadow: '0 14px 40px rgba(0,0,0,0.6), inset 0 1px 0 rgba(255,255,255,0.06)',
        padding: 16,
        display:'flex', alignItems:'center', gap: 14,
        fontFamily: 'var(--mono)',
      }}>
        {/* screen bezel */}
        <div style={{
          flex:1, height: H - 32, background:'#07090a', borderRadius: 4,
          border: '1px solid #000',
          boxShadow: 'inset 0 0 20px rgba(0,0,0,0.8)',
          padding: '10px 14px', display:'flex', flexDirection:'column', justifyContent:'space-between',
          position:'relative', overflow:'hidden',
        }}>
          {/* faint scanlines */}
          <div style={{
            position:'absolute', inset:0, pointerEvents:'none',
            backgroundImage:'repeating-linear-gradient(0deg, rgba(255,255,255,0.03) 0 1px, transparent 1px 3px)',
          }}/>
          {/* top row: label + big number */}
          <div style={{display:'flex', alignItems:'center', justifyContent:'space-between', position:'relative'}}>
            <span style={{
              fontFamily:'var(--pixel)', fontSize: 18, color: toneColor, letterSpacing: 2,
              textShadow: `0 0 8px ${toneColor}99`,
            }}>{m.label}</span>
            <div style={{display:'flex', alignItems:'center'}}>
              <SevenSeg digit={valStr[0]} size={14} on={toneColor}/>
              <SevenSeg digit={valStr[1]} size={14} on={toneColor}/>
              <span style={{
                fontFamily:'var(--pixel)', fontSize: 18, color: toneColor, marginLeft: 6,
                letterSpacing: 1, textShadow:`0 0 6px ${toneColor}99`,
              }}>{m.unit.slice(0,1)}</span>
            </div>
          </div>
          {/* bar */}
          <PixelBar value={m.value} color={toneColor} height={14} segments={22}/>
          {/* bottom row: reset */}
          <div style={{display:'flex', justifyContent:'space-between',
            fontFamily:'var(--pixel)', fontSize: 14, color: `${toneColor}aa`,
            letterSpacing: 1, textShadow:`0 0 4px ${toneColor}66`,
          }}>
            <span>{m.unit.slice(1).trim()}</span>
            <span>RST {m.reset}</span>
          </div>
        </div>
        {/* tap button on the right */}
        <div style={{
          width: 26, height: 26, borderRadius:'50%',
          background:'radial-gradient(circle at 30% 30%, #4a4336, #1a1713)',
          boxShadow:'inset 0 -2px 4px rgba(0,0,0,0.6), 0 1px 2px rgba(255,255,255,0.04)',
          border:'1px solid #3a3528',
        }}/>
      </div>

      {showAnnotations && (
        <>
          <div className="annot" style={{top: -18, left: 60, fontSize: 9, color:'#e8a020'}}>
            <span className="lab">OLED 0.97" · 128×32</span>
          </div>
          <div className="annot" style={{bottom: -22, right: 0, fontSize: 9, color:'#e8a020'}}>
            <span className="lab">TAP → CYCLE METRICS</span>
          </div>
          <div className="annot" style={{top: 40, right: -140, fontSize: 9, color:'#e8a020'}}>
            <span className="lab">MOMENTARY BTN · SILICONE CAP</span>
          </div>
          {/* leader lines */}
          <svg style={{position:'absolute', inset:0, width:'100%', height:'100%', overflow:'visible', pointerEvents:'none'}}>
            <line x1="130" y1="-6" x2="130" y2="20" stroke="#e8a02088" strokeDasharray="2 2"/>
            <line x1={W*scale - 32} y1={H*scale*0.5} x2={W*scale + 20} y2={H*scale*0.5} stroke="#e8a02088" strokeDasharray="2 2"/>
          </svg>
        </>
      )}
    </div>
  );
}

// PCB silkscreen mini-mockup
function PCBSvg() {
  return (
    <svg viewBox="0 0 400 200" width="100%" height="100%" style={{display:'block'}}>
      <rect width="400" height="200" fill="#0b3d2e"/>
      {/* copper traces */}
      <g stroke="#d4a84a" strokeWidth="1.5" fill="none" opacity="0.7">
        <path d="M 30 30 L 30 80 L 90 80 L 90 150"/>
        <path d="M 60 30 L 60 110 L 140 110 L 140 170"/>
        <path d="M 120 30 L 120 60 L 200 60"/>
        <path d="M 370 30 L 370 80 L 310 80 L 310 150"/>
        <path d="M 340 30 L 340 110 L 260 110 L 260 170"/>
        <path d="M 280 30 L 280 60 L 220 60"/>
        <path d="M 50 180 L 350 180"/>
      </g>
      {/* pads */}
      <g fill="#d4a84a">
        {[30,60,90,120,150,180,210,240,270,300,330,360,370].map((x,i)=>(
          <rect key={i} x={x-3} y="18" width="6" height="10" rx="1"/>
        ))}
      </g>
      {/* MCU */}
      <rect x="150" y="70" width="100" height="60" fill="#1a1a1a" stroke="#d4a84a" strokeWidth="1"/>
      <text x="200" y="95" textAnchor="middle" fill="#fff" fontFamily="monospace" fontSize="8">ESP32-C3</text>
      <text x="200" y="108" textAnchor="middle" fill="#888" fontFamily="monospace" fontSize="6">U1</text>
      <text x="200" y="120" textAnchor="middle" fill="#888" fontFamily="monospace" fontSize="6">ESPRESSIF</text>
      {/* caps / resistors */}
      <g fill="#c0c0c0" stroke="#888">
        <rect x="80" y="60" width="12" height="6"/>
        <rect x="300" y="60" width="12" height="6"/>
        <rect x="80" y="130" width="12" height="6"/>
        <rect x="308" y="130" width="12" height="6"/>
      </g>
      {/* silkscreen */}
      <g fill="#ffffff" fontFamily="monospace" fontSize="7" opacity="0.8">
        <text x="14" y="14">LIMITBAR-MAIN v0.3</text>
        <text x="386" y="14" textAnchor="end">Cfomodz · 2026</text>
        <text x="14" y="196">USB-C</text>
        <text x="386" y="196" textAnchor="end">OLED FPC</text>
      </g>
      {/* mounting holes */}
      <circle cx="14" cy="100" r="4" fill="#0b3d2e" stroke="#d4a84a"/>
      <circle cx="386" cy="100" r="4" fill="#0b3d2e" stroke="#d4a84a"/>
    </svg>
  );
}

// Exploded iso-ish diagram using layered divs
function Exploded() {
  const layers = [
    { name: 'BEZEL', color: '#3a3528', label:'AL-6061' },
    { name: 'GLASS', color: '#4bb8c733', label:'AR-coated' },
    { name: 'OLED',  color: '#0a0a0a', label:'128×32' },
    { name: 'PCB',   color: '#0b3d2e', label:'v0.3' },
    { name: 'BASE',  color: '#2a2520', label:'CNC shell' },
  ];
  return (
    <div style={{position:'relative', width:'100%', aspectRatio:'4/3', perspective: 900}}>
      <svg viewBox="0 0 400 300" width="100%" height="100%" style={{position:'absolute', inset:0}}>
        <line x1="200" y1="20" x2="200" y2="280" stroke="#e8a02055" strokeDasharray="3 3"/>
      </svg>
      {layers.map((L, i) => (
        <div key={i} style={{
          position:'absolute', left:'50%', top: `${10 + i*16}%`,
          width: '55%',
          transform: `translateX(-50%) rotateX(58deg) rotateZ(-12deg)`,
          transformStyle:'preserve-3d',
          background: L.color, height: 38, border: '1px solid #e8a02055',
          boxShadow: '0 4px 18px rgba(0,0,0,0.5)',
          display:'flex', alignItems:'center', justifyContent:'center',
        }}>
          <span style={{
            fontFamily:'var(--mono)', fontSize: 10, color:'#e8a020',
            letterSpacing: 2, transform:'rotateZ(0)',
          }}>{L.name}</span>
        </div>
      ))}
      {/* annotations: left side for odd layers, right side for even, so they can't collide */}
      {layers.map((L, i) => {
        const right = i % 2 === 0;
        return (
          <div key={'l'+i} className="annot" style={{
            [right ? 'left' : 'right']: '4%',
            top: `${13 + i*16}%`, fontSize: 9, color:'#e8a020', whiteSpace:'nowrap',
          }}>
            <span className="lab">{String(i+1).padStart(2,'0')} · {L.name}{L.label ? ' · '+L.label : ''}</span>
          </div>
        );
      })}
    </div>
  );
}

Object.assign(window, { Device, PixelBar, SevenSeg, PCBSvg, Exploded, METRICS });
