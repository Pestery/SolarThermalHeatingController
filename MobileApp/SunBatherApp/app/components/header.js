export function onNavigateHome(args) {
    console.log("home");
    const view = args.object;
    const page = view.page;
    page.frame.navigate("home/index");
}

export function onNavigateGraphs(args) {
    console.log("Graphs"); 
    const view = args.object;
    const page = view.page;
    page.frame.navigate("graphs/graph");
}

export function onNavigateController(args) {
    console.log("Controller"); 
    const view = args.object;
    const page = view.page;
    //const tappedItem = view.bindingContext -- can add context
  
    page.frame.navigate({
      moduleName: 'controller/controller',
      //context: tappedItem,
      animated: true,
      transition: {
        name: 'slide',
        duration: 200,
        curve: 'ease',
      },
    })
}
 
export function onNavigateConnect(args) {
    console.log("Connect"); 
    const view = args.object;
    const page = view.page;
    page.frame.navigate("connect/connect");
}