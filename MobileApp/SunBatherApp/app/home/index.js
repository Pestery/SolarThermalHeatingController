import { UserViewModel } from './index-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new UserViewModel()
}

export function onNavigateGraphs(args) {
  console.log("Graphs"); 
  const view = args.object;
  const page = view.page;
  page.frame.navigate("graphs/graph");
}