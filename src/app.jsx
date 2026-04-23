const DEFAULT_TWEAKS = /*EDITMODE-BEGIN*/{
  "accent": "amber",
  "metric": 0,
  "grid": "on"
}/*EDITMODE-END*/;

function App() {
  const [state, setState] = React.useState(DEFAULT_TWEAKS);
  const [metric, setMetric] = React.useState(DEFAULT_TWEAKS.metric);

  // Apply tweak state to <body> attributes
  React.useEffect(() => {
    document.body.setAttribute('data-accent', state.accent);
    document.body.setAttribute('data-grid', state.grid);
    setMetric(state.metric);
  }, [state]);

  // Auto-cycle hint: not running by default; user can click cards.

  return (
    <>
      <TopBar/>
      <Hero metric={metric} setMetric={setMetric}/>
      <HowItWorks metric={metric} setMetric={setMetric}/>
      <Parts/>
      <DevLog/>
      <Specs/>
      <FAQ/>
      <Signup/>
      <Footer/>
      <Tweaks state={state} setState={setState}/>
    </>
  );
}

ReactDOM.createRoot(document.getElementById('root')).render(<App/>);
