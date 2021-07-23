import { ContollerViewModel } from './controller-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new ContollerViewModel()
}

export function onNavigateConnect(args) {
  console.log("Connect"); 
  const view = args.object;
  const page = view.page;
  page.frame.navigate("connect/connect");
}