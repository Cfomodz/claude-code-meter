// Page sections

function TopBar() {
  return (
    <div className="top">
      <div className="brand"><div className="sq"/><span>LIMITBAR // v0.3-proto</span></div>
      <div className="links">
        <a href="#how">How it works</a>
        <a href="#parts">Parts</a>
        <a href="#log">Dev log</a>
        <a href="#specs">Specs</a>
        <a href="#faq">FAQ</a>
      </div>
      <div className="status"><div className="dot"/><span>notify me · kickstarter tbd</span></div>
    </div>
  );
}

function Hero({ metric, setMetric }) {
  return (
    <section>
      <div className="wrap">
        <div className="hero">
          <div>
            <div className="eyebrow">// a desk-side meter for claude code · prototype v0.3</div>
            <h1>
              GLANCE.<br/>
              DON'T<br/>
              <span className="acc">STARE.</span>
            </h1>
            <p className="lede">
              A palm-sized display that sits beside your keyboard and shows one thing:
              how much Claude Code you have left. Session, week, or overage — one tap
              cycles the view. No notifications. No dashboards. Just the number.
            </p>
            <div className="hero-cta">
              <a href="#signup" className="btn">Notify me →</a>
              <a href="#how" className="btn ghost">See how it works</a>
              <div className="stamp">unreleased · kickstarter soon</div>
            </div>
            <div className="meta">
              <div><b>0.97"</b>OLED height</div>
              <div><b>3.12"</b>width (79mm)</div>
              <div><b>USB-C</b>no app required</div>
              <div><b>$39</b>target MSRP</div>
            </div>
          </div>
          <div style={{position:'relative'}}>
            <div className="reg tl"><div className="circ"/></div>
            <div className="reg tr"><div className="circ"/></div>
            <div className="reg bl"><div className="circ"/></div>
            <div className="reg br"><div className="circ"/></div>
            <BeforeAfter metric={metric} setMetric={setMetric}/>
            <div className="callout" style={{marginTop: 14, justifyContent:'space-between', display:'flex'}}>
              <span>fig.01 — design intent ↔ bench prototype</span>
              <span>drag →</span>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

function BeforeAfter({ metric }) {
  const [split, setSplit] = React.useState(52);
  const ref = React.useRef(null);
  const dragging = React.useRef(false);
  const onDown = (e) => { dragging.current = true; e.preventDefault(); };
  const onMove = (e) => {
    if (!dragging.current || !ref.current) return;
    const rect = ref.current.getBoundingClientRect();
    const x = (e.touches ? e.touches[0].clientX : e.clientX) - rect.left;
    setSplit(Math.max(2, Math.min(98, (x/rect.width)*100)));
  };
  const onUp = () => { dragging.current = false; };
  React.useEffect(() => {
    window.addEventListener('mousemove', onMove); window.addEventListener('mouseup', onUp);
    window.addEventListener('touchmove', onMove); window.addEventListener('touchend', onUp);
    return () => {
      window.removeEventListener('mousemove', onMove); window.removeEventListener('mouseup', onUp);
      window.removeEventListener('touchmove', onMove); window.removeEventListener('touchend', onUp);
    };
  }, []);

  return (
    <div className="ba" ref={ref} style={{'--split': split+'%'}}>
      <div className="layer wireframe">
        <div style={{display:'grid', placeItems:'center', height:'100%'}}>
          <Device metric={metric} mode="wireframe" scale={1.2}/>
        </div>
        <div className="tag l">01 · DESIGN</div>
      </div>
      <div className="layer after" style={{background:'#0d0c0a'}}>
        <div className="placeholder" style={{position:'absolute', inset:0}}>
          {/* render device on top of placeholder tone */}
        </div>
        <div style={{position:'absolute', inset:0, display:'grid', placeItems:'center'}}>
          <Device metric={metric} mode="live" scale={1.2}/>
        </div>
        <div className="tag r">02 · BENCH</div>
      </div>
      <div className="handle" onMouseDown={onDown} onTouchStart={onDown}/>
    </div>
  );
}

function HowItWorks({ metric, setMetric }) {
  return (
    <section id="how">
      <div className="wrap">
        <div className="sec-head">
          <div>
            <div className="sec-num">§ 01 / Interaction</div>
            <h2 className="sec-title">One tap.<br/>Three views.</h2>
          </div>
          <div className="sec-sub" style={{maxWidth: 340}}>
            A single momentary button cycles between the three metrics you actually care about.
            No menus, no scrolling, no screens within screens.
          </div>
        </div>

        <div className="g3" style={{gap: 18}}>
          {METRICS.map((m, i) => (
            <div
              key={m.id}
              className="card"
              onClick={() => setMetric(i)}
              style={{
                cursor:'pointer',
                outline: metric === i ? '1px solid var(--accent)' : 'none',
                transition:'outline 0.2s',
              }}
            >
              <div style={{display:'flex', justifyContent:'space-between', marginBottom: 14}}>
                <span className="mono dim" style={{fontSize: 10, letterSpacing:'0.2em'}}>MODE 0{i+1}</span>
                <span className="mono hl" style={{fontSize: 10, letterSpacing:'0.2em'}}>{metric === i ? 'ACTIVE' : 'TAP'}</span>
              </div>
              <h3 className="pixel" style={{fontSize: 32, color:'var(--paper)', margin:'0 0 6px', lineHeight: 1}}>{m.full}</h3>
              <p className="mono" style={{fontSize: 12, color:'#bfb193', lineHeight: 1.6, margin:'4px 0 18px'}}>
                {i === 0 && 'Remaining capacity in the current 5-hour rolling window. Bar drains as you work.'}
                {i === 1 && 'Seven-day rolling ceiling. The big-picture budget.'}
                {i === 2 && 'Dollars accrued past your plan allowance, updated each billing cycle.'}
              </p>
              <div style={{transform:'scale(0.72)', transformOrigin:'left top', height: 100}}>
                <Device metric={i} mode="live" scale={0.72}/>
              </div>
            </div>
          ))}
        </div>

        <div className="callout" style={{marginTop: 36}}>fig.02 — click any card to preview that mode on the hero device</div>
      </div>
    </section>
  );
}

function Parts() {
  const parts = [
    { n: 'A-1', name: 'OLED STRIP',    spec: ['0.97"', '128×32', 'SSD1306', 'I²C'], desc: 'Low-res by design. Readable at arm\'s length, dim enough to fade into the desk.', viz: 'oled' },
    { n: 'B-2', name: 'MAIN BOARD',    spec: ['ESP32-C3', 'Wi-Fi', '4MB flash', 'USB-C'], desc: 'Polls the Anthropic usage API every 5 min over Wi-Fi. Credentials paired once via captive portal.', viz: 'pcb' },
    { n: 'C-3', name: 'ENCLOSURE',     spec: ['CNC AL-6061', 'blasted', 'bead-black', '79×25×18'], desc: 'Machined aluminum shell. Rubber feet. Weighted enough to not slide when you tap it.', viz: 'enc' },
    { n: 'D-4', name: 'TAP BUTTON',    spec: ['momentary', 'silicone cap', '1 N actuation'], desc: 'Flush-mount. Soft click. No detent — tap pattern is the only input.', viz: 'btn' },
  ];
  return (
    <section id="parts">
      <div className="wrap">
        <div className="sec-head">
          <div>
            <div className="sec-num">§ 02 / Bill of materials</div>
            <h2 className="sec-title">Four parts.<br/>That's it.</h2>
          </div>
          <div className="sec-sub" style={{maxWidth: 340}}>
            Deliberately boring internals. A common MCU, a common display, a machined shell.
            Serviceable, sourceable, cheap to iterate.
          </div>
        </div>
        <div className="g4">
          {parts.map(p => (
            <div key={p.n} className="part">
              <div style={{display:'flex', justifyContent:'space-between'}}>
                <span className="num">{p.n}</span>
                <span className="num">QTY · 1</span>
              </div>
              <div className="viz">
                {p.viz === 'pcb' && <PCBSvg/>}
                {p.viz === 'oled' && <OledViz/>}
                {p.viz === 'enc' && <EncViz/>}
                {p.viz === 'btn' && <BtnViz/>}
              </div>
              <div className="name">{p.name}</div>
              <div className="desc">{p.desc}</div>
              <div className="specs">{p.spec.map(s => <span key={s}>{s}</span>)}</div>
            </div>
          ))}
        </div>

        <div style={{marginTop: 56}} className="g2">
          <div className="card" style={{padding: 0, overflow:'hidden'}}>
            <div style={{aspectRatio:'16/10'}}>
              <Exploded/>
            </div>
            <div style={{padding: '14px 18px', borderTop:'1px solid var(--line)', display:'flex', justifyContent:'space-between'}}>
              <span className="mono dim" style={{fontSize: 11, letterSpacing:'0.15em', textTransform:'uppercase'}}>fig.03 — exploded iso</span>
              <span className="mono hl" style={{fontSize: 11, letterSpacing:'0.15em', textTransform:'uppercase'}}>5 layers · 12mm stack</span>
            </div>
          </div>
          <div className="card" style={{padding: 0}}>
            <div style={{aspectRatio:'16/10', padding: 20, display:'flex', alignItems:'center', justifyContent:'center', background:'#0a0f14'}}>
              <Device metric={0} mode="wireframe" scale={1.3}/>
            </div>
            <div style={{padding: '14px 18px', borderTop:'1px solid var(--line)', display:'flex', justifyContent:'space-between'}}>
              <span className="mono dim" style={{fontSize: 11, letterSpacing:'0.15em', textTransform:'uppercase'}}>fig.04 — top view · dimensioned</span>
              <span className="mono hl" style={{fontSize: 11, letterSpacing:'0.15em', textTransform:'uppercase'}}>79 × 25 mm</span>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

function OledViz() {
  return (
    <div style={{width:'100%', height:'100%', display:'grid', placeItems:'center', background:'#0a0a0a'}}>
      <div style={{width:'80%', height:'40%', background:'#000', border:'1px solid #222', position:'relative', padding: 6}}>
        <div style={{display:'flex', gap: 1, height: '100%'}}>
          {Array.from({length: 32}).map((_,i) => (
            <div key={i} style={{flex:1, background: i < 20 ? '#e8a020' : '#e8a02022', boxShadow: i < 20 ? '0 0 2px #e8a020' : 'none'}}/>
          ))}
        </div>
      </div>
    </div>
  );
}
function EncViz() {
  return (
    <svg viewBox="0 0 200 150" width="100%" height="100%">
      <defs>
        <linearGradient id="alu" x1="0" x2="0" y1="0" y2="1">
          <stop offset="0" stopColor="#2a2520"/><stop offset="0.5" stopColor="#4a4336"/><stop offset="1" stopColor="#1a1713"/>
        </linearGradient>
      </defs>
      <rect width="200" height="150" fill="#0a0a0a"/>
      <path d="M 30 55 L 170 55 L 190 75 L 190 105 L 170 125 L 30 125 L 10 105 L 10 75 Z" fill="url(#alu)" stroke="#e8a02055"/>
      <rect x="40" y="70" width="120" height="40" fill="#000" stroke="#e8a02055"/>
      <circle cx="150" cy="90" r="4" fill="#e8a020" opacity="0.5"/>
      <text x="100" y="145" textAnchor="middle" fontFamily="monospace" fontSize="8" fill="#8a7b5e">ISO · CNC · AL-6061</text>
    </svg>
  );
}
function BtnViz() {
  return (
    <svg viewBox="0 0 200 150" width="100%" height="100%">
      <rect width="200" height="150" fill="#0a0a0a"/>
      <circle cx="100" cy="75" r="44" fill="none" stroke="#e8a02055" strokeDasharray="2 3"/>
      <circle cx="100" cy="75" r="30" fill="#1a1713" stroke="#e8a020"/>
      <circle cx="100" cy="75" r="22" fill="#2a2520"/>
      <circle cx="92" cy="68" r="4" fill="#4a4336"/>
      <text x="100" y="140" textAnchor="middle" fontFamily="monospace" fontSize="8" fill="#8a7b5e">MOMENTARY · SILICONE</text>
    </svg>
  );
}

function DevLog() {
  const entries = [
    { date: 'MAR 08', title: 'Sketch phase',
      body: "First napkin: put it next to the keyboard, make it one line, make it boring. I don't want to look at a dashboard. I want to glance at a bar.",
      tags: ['sketch','v0.1'] },
    { date: 'MAR 14', title: 'Breadboard works',
      body: "ESP32-C3 pulling from the OAuth usage endpoint every 5 min. Display scrolls. Button cycles modes. Wife says it looks like a calculator from 1986 — that's the goal.",
      tags: ['firmware','v0.2'] },
    { date: 'APR 01', title: 'PCB fab v0.3 in transit',
      body: "Moved from perfboard to real PCB. Shrunk the footprint by 60%. Moved to USB-C. Added mounting holes for the aluminum shell. Pending: JLCPCB ships Thursday.",
      tags: ['pcb','v0.3','fabbed'] },
    { date: 'APR 12', title: 'Readability testing',
      body: "Can I read the bar at arm's length without turning my head? Yes at 50% brightness. Ambient reference, not alarm — rule #1 of this thing.",
      tags: ['ux'] },
  ];
  return (
    <section id="log">
      <div className="wrap">
        <div className="sec-head">
          <div>
            <div className="sec-num">§ 03 / Dev log</div>
            <h2 className="sec-title">Built in public.<br/>Mostly in the garage.</h2>
          </div>
          <div className="sec-sub" style={{maxWidth: 340}}>
            Honest snapshots from the bench. No product launch video. Yet.
          </div>
        </div>

        <div className="g2" style={{marginBottom: 48}}>
          <div className="placeholder" style={{aspectRatio:'4/3'}}>
            BREADBOARD PHOTO<br/>
            <span className="kicker">[drop: esp32 + ssd1306 on white cutting mat]</span>
          </div>
          <div className="placeholder" style={{aspectRatio:'4/3'}}>
            PCB v0.3 RENDER<br/>
            <span className="kicker">[drop: top + bottom copper · kicad export]</span>
          </div>
        </div>

        <div>
          {entries.map(e => (
            <div key={e.title} className="log">
              <div className="date"><b>{e.date}</b>2026</div>
              <div>
                <h3>{e.title}</h3>
                <p>{e.body}</p>
                <div className="tags">{e.tags.map(t => <span key={t}>#{t}</span>)}</div>
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}

function Specs() {
  return (
    <section id="specs">
      <div className="wrap">
        <div className="sec-head">
          <div>
            <div className="sec-num">§ 04 / Specification · DRAFT</div>
            <h2 className="sec-title">Numbers, if<br/>you're curious.</h2>
          </div>
          <div className="sec-sub" style={{maxWidth: 340}}>
            Subject to change until the first run ships. Red entries are what I'm still figuring out.
          </div>
        </div>
        <div className="spec">
          {[
            ['Dimensions', '79 × 25 × 18 mm', '3.12 × 0.97 × 0.71 in'],
            ['Weight',     '58 g',            'aluminum shell · rubber feet'],
            ['Display',    '0.97" OLED',      '128×32 · monochrome amber'],
            ['MCU',        'ESP32-C3',        'Wi-Fi 802.11 b/g/n'],
            ['Connection', 'USB-C',           'power only · no data required'],
            ['Refresh',    '5 minutes',       'matches Anthropic API cadence'],
            ['Auth',       'OAuth',           'paired via phone captive portal'],
            ['Price',      'target $39',      '< $49 @ scale · final tbd', true],
            ['Ship',       'Q4 2026',         'if funded · tbd', true],
          ].map(([k,v,n,red]) => (
            <div key={k} className="row">
              <div className="k">{k}</div>
              <div className="v">{v}</div>
              <div className={red ? 'note' : 'v'} style={{color: red ? 'var(--red)' : '#8a7b5e'}}>{n}</div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}

function FAQ() {
  const qs = [
    ['Is this a real product I can buy?', 'Not yet. This page exists to gauge whether enough people want it to justify a Kickstarter. Drop your email below if you do.'],
    ['Does it work with a Claude plan I already have?', 'Yes. It uses the same OAuth that the Claude Code CLI already uses — no separate account, no extra subscription. You pair it once.'],
    ['Does it store or send my data anywhere?', 'No. It talks to Anthropic directly, same endpoint the CLI hits. I never see anything.'],
    ['Why not just a menu-bar app?', 'Because a menu-bar app lives on the screen I\'m already ignoring when I\'m in flow. This sits in my peripheral vision and demands nothing.'],
    ['Will there be a non-Claude version?', 'Maybe. The hardware is generic — it could show GitHub Actions minutes, OpenAI credits, your own Grafana number. If there\'s interest.'],
    ['Can I hack it?', 'Yes. Source firmware will be MIT. USB-C flashing. Breakout pads on the back for anything you want.'],
  ];
  return (
    <section id="faq">
      <div className="wrap">
        <div className="sec-head">
          <div>
            <div className="sec-num">§ 05 / Questions</div>
            <h2 className="sec-title">Things people<br/>have asked.</h2>
          </div>
        </div>
        <div className="faq">
          {qs.map(([q,a]) => (
            <details key={q}><summary>{q}</summary><p>{a}</p></details>
          ))}
        </div>
      </div>
    </section>
  );
}

function Signup() {
  const [sent, setSent] = React.useState(false);
  return (
    <section id="signup">
      <div className="wrap">
        <div className="signup">
          <div className="eyebrow mono dim" style={{fontSize: 11, letterSpacing:'0.2em', textTransform:'uppercase', marginBottom: 10}}>// if enough of you want this, I'll build it</div>
          <h2>Tell me you<br/>want <span className="acc">one.</span></h2>
          <p>Drop your email and I'll send a single message when the Kickstarter goes live. Nothing else. No newsletter, no launch-funnel, no tracking pixel.</p>
          {sent ? (
            <div className="mono hl" style={{fontSize: 14, letterSpacing:'0.15em', textTransform:'uppercase'}}>✓ LOGGED · TALK SOON</div>
          ) : (
            <form onSubmit={(e)=>{e.preventDefault(); setSent(true);}}>
              <input type="email" required placeholder="you@domain · for one (1) email, ever"/>
              <button className="btn" type="submit">Notify me →</button>
            </form>
          )}
          <div className="disc">≈ 412 signed up · goal 1,000 before I order 500 shells</div>
        </div>
      </div>
    </section>
  );
}

function Footer() {
  return (
    <footer>
      <div className="inner">
        <div>© 2026 · LIMITBAR · a project by one person · not affiliated with Anthropic</div>
        <div>v0.3-proto · last updated APR 17</div>
      </div>
    </footer>
  );
}

Object.assign(window, { TopBar, Hero, HowItWorks, Parts, DevLog, Specs, FAQ, Signup, Footer });
